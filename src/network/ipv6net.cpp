#include <libnet/network/ipv6net.hpp>
#include <stdexcept>

libnet::IPv6Net::IPv6Net(std::string_view cidr, size_t pool_size) {
    size_t slash = cidr.find('/');
    _prefix = (slash != std::string_view::npos) ? 
               static_cast<uint8_t>(std::stoi(std::string(cidr.substr(slash + 1)))) : 128;

    if (_prefix > 128) throw std::invalid_argument("Prefix too large");

    uint128_t mask = CalculateMask(_prefix);
    uint128_t raw_ip = IPv6(cidr.substr(0, slash)).GetRawAddress();
    _base = IPv6(raw_ip & mask, 0);

    // В IPv6 мы не можем сделать bitset на всю сеть /64, 
    // поэтому инициализируем пул фиксированного размера.
    _pool = std::make_unique<libnet::bitset>(pool_size);

    // Резервируем "нулевой" адрес (Anycast роутера)
    _pool->set(0);
}

libnet::uint128_t libnet::IPv6Net::CalculateMask(uint8_t prefix) const noexcept {
    if (prefix == 0) return uint128_t(0);
    if (prefix >= 128) return ~uint128_t(0);

    uint64_t high = 0, low = 0;
    if (prefix <= 64) {
        // Используем 0ULL чтобы гарантировать 64 бита. Сдвиг на 64 — это UB, поэтому проверка
        high = (prefix == 0) ? 0 : (~0ULL << (64 - prefix));
        low = 0;
    } else {
        high = ~0ULL;
        low = ~0ULL << (128 - prefix);
    }
    
    return uint128_t(high, low);
}



bool libnet::IPv6Net::Contains(const IPv6& ip) const noexcept {
    uint128_t mask = CalculateMask(_prefix);
    // Сравниваем uint128_t с uint128_t
    return (ip.GetRawAddress() & mask) == _base.GetRawAddress();
}

bool libnet::IPv6Net::IsFree(const IPv6& ip) const noexcept {
    if (!Contains(ip)) return false;
    
    uint128_t offset = ip.GetRawAddress().raw() - _base.GetRawAddress().raw();
    // Если адрес за пределами нашего отслеживаемого пула — считаем занятым
    if (offset >= uint128_t(_pool->get_bits_count())) return false;

    std::lock_guard<std::mutex> lock(_mutex);
    return !_pool->test(static_cast<size_t>(offset.low()));
}

libnet::IPv6 libnet::IPv6Net::Allocate() {
    std::lock_guard<std::mutex> lock(_mutex);
    auto offset = _pool->FindFirstFree();
    if (!offset) {
        throw std::runtime_error("IPv6 Pool exhausted (limit reached)");
    }

    _pool->set(*offset);
    return IPv6(_base.GetRawAddress() + uint128_t(*offset), 0);
}

void libnet::IPv6Net::Release(const IPv6& ip) noexcept {
    if (Contains(ip)) {
        uint128_t offset = ip.GetRawAddress().raw() - _base.GetRawAddress().raw();
        if (offset < _pool->get_bits_count()) {
            std::lock_guard<std::mutex> lock(_mutex);
            _pool->reset(static_cast<size_t>(offset.low()));
        }
    }
}

void libnet::IPv6Net::Reserve(const IPv6& ip) {
    if(!Contains(ip)) {
        throw std::runtime_error("IP address is outside of the network range");
    }

    uint128_t offset = ip.GetRawAddress().raw() - _base.GetRawAddress().raw();
    if (offset >= _pool->get_bits_count()) {
        throw std::runtime_error("Address is outside of allocatable pool range");
    }

    std::lock_guard<std::mutex> lock(_mutex);
    _pool->set(static_cast<size_t>(offset.low()));
}

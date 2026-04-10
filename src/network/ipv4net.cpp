#include <libnet/network/ipv4net.hpp>
#include <stdexcept>

libnet::IPv4Net::IPv4Net(std::string_view cidr) {
    size_t slash = cidr.find('/');
    _prefix = (slash != std::string_view::npos) ? 
               static_cast<uint8_t>(std::stoi(std::string(cidr.substr(slash + 1)))) : 32;

    if (_prefix > 32) throw std::invalid_argument("Prefix too large");

    uint32_t mask = (_prefix == 0) ? 0 : (~0U << (32 - _prefix));
    uint32_t raw_ip = IPv4(cidr.substr(0, slash)).GetRawAddress();
    _base = IPv4(raw_ip & mask, 0);

    // Количество адресов в сети
    uint64_t host_count = 1ULL << (32 - _prefix);
    _pool = std::make_unique<libnet::bitset>(host_count);

    // В сетях меньше /31 первый и последний адреса зарезервированы (Net & Broadcast)
    if (_prefix < 31) {
        _pool->set(0);               // Network ID
        _pool->set(host_count - 1);  // Broadcast
    }
}

bool libnet::IPv4Net::Contains(const IPv4& ip) const noexcept {
    uint32_t mask = (_prefix == 0) ? 0 : (~0U << (32 - _prefix));
    return (ip.GetRawAddress() & mask) == _base.GetRawAddress();
}

bool libnet::IPv4Net::IsFree(const IPv4& ip) const noexcept {
    if (!Contains(ip)) return false;
    std::lock_guard<std::mutex> lock(_mutex);
    uint32_t offset = ip.GetRawAddress() - _base.GetRawAddress();
    return !_pool->test(offset);
}

libnet::IPv4 libnet::IPv4Net::Allocate() {
    std::lock_guard<std::mutex> lock(_mutex);
    auto offset = _pool->FindFirstFree();
    if (!offset) {
        throw std::runtime_error("No free IP addresses in network");
    }

    _pool->set(*offset);
    return IPv4(_base.GetRawAddress() + static_cast<uint32_t>(*offset), 0);
}

void libnet::IPv4Net::Release(const IPv4& ip) noexcept {
    if (Contains(ip)) {
        std::lock_guard<std::mutex> lock(_mutex);
        uint32_t offset = ip.GetRawAddress() - _base.GetRawAddress();
        _pool->reset(offset);
    }
}

void libnet::IPv4Net::Reserve(const IPv4& ip) {
    if(!Contains(ip)) {
        throw std::runtime_error("IP address is outside of the network range");
    }

    std::lock_guard<std::mutex> lock(_mutex);

    uint32_t offset = ip.GetRawAddress() - _base.GetRawAddress();
    _pool->set(offset);
}
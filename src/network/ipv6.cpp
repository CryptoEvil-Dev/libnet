#include <libnet/network/ipv6.hpp>
#include <arpa/inet.h>
#include <cstring>

libnet::IPv6::IPv6(const char* address) {
    std::string addr_str(address);
    std::string ip_part = addr_str;
    
    // Проверка на наличие порта [addr]:port
    if (addr_str.front() == '[') {
        size_t bracket_end = addr_str.find(']');
        if (bracket_end != std::string::npos) {
            ip_part = addr_str.substr(1, bracket_end - 1);
            size_t colon_port = addr_str.find(':', bracket_end);
            if (colon_port != std::string::npos) {
                _port = static_cast<uint16_t>(std::stoul(addr_str.substr(colon_port + 1)));
            }
        }
    }

    in6_addr tmp_addr;
    if (inet_pton(AF_INET6, ip_part.c_str(), &tmp_addr) == 1) {
        // Копируем 16 байт в наш uint128_t (учитываем big-endian сети)
        uint64_t high, low;
        std::memcpy(&high, &tmp_addr.s6_addr[0], 8);
        std::memcpy(&low, &tmp_addr.s6_addr[8], 8);
        
        // Преобразуем из сетевого порядка байт в хостовый
        _addr = uint128_t(be64toh(high), be64toh(low));
    }
}

libnet::IPv6::IPv6(const sockaddr_in6& sa) noexcept {
    _port = ntohs(sa.sin6_port);

    uint64_t high, low;
    std::memcpy(&high, &sa.sin6_addr.s6_addr[0], 8);
    std::memcpy(&low, &sa.sin6_addr.s6_addr[8], 8);

    // Переводим из сетевого порядка (Big Endian) в хостовой
    _addr = uint128_t(be64toh(high), be64toh(low));
}

std::string libnet::IPv6::GetAddress() const noexcept {
    char buf[INET6_ADDRSTRLEN];
    in6_addr tmp_addr;
    
    uint64_t high = htobe64(_addr.high());
    uint64_t low = htobe64(_addr.low());
    
    std::memcpy(&tmp_addr.s6_addr[0], &high, 8);
    std::memcpy(&tmp_addr.s6_addr[8], &low, 8);
    
    if (inet_ntop(AF_INET6, &tmp_addr, buf, sizeof(buf))) {
        return {buf};
    }
    return "::";
}

std::string libnet::IPv6::GetFullAddress() const noexcept {
    if (_port > 0) {
        return "[" + GetAddress() + "]:" + std::to_string(_port);
    }
    return GetAddress();
}

sockaddr_in6 libnet::IPv6::GetSockAddr() const noexcept {
    sockaddr_in6 addr{};
    addr.sin6_family = AF_INET6;
    addr.sin6_port = htons(_port);
    
    uint64_t high = htobe64(_addr.high());
    uint64_t low = htobe64(_addr.low());
    std::memcpy(&addr.sin6_addr.s6_addr[0], &high, 8);
    std::memcpy(&addr.sin6_addr.s6_addr[8], &low, 8);
    
    return addr;
}

#pragma once

#include <libnet/core/uint128.hpp>
#include <netinet/in.h>
#include <string_view>
#include <string>

namespace libnet {
    class IPv6 {
    private:
        uint128_t _addr {0};
        uint16_t _port {0};

    public:
        IPv6() = default;
        IPv6(const char* address);
        IPv6(std::string_view addr) : IPv6(std::string(addr).c_str()) {}
        IPv6(const uint128_t& address, uint16_t port) noexcept 
            : _addr(address), _port(port) {}

        bool operator == (const IPv6& other) const noexcept {
            return _addr == other._addr && _port == other._port;
        }

        std::string GetFullAddress() const noexcept;
        std::string GetAddress() const noexcept;
        uint16_t GetPort() const noexcept { return _port; }
        uint128_t GetRawAddress() const noexcept { return _addr; }

        sockaddr_in6 GetSockAddr() const noexcept;
    };

    inline std::ostream& operator << (std::ostream& os, const IPv6& ip) {
        os << ip.GetFullAddress();
        return os;
    }
}

namespace std {
    template<> struct hash<libnet::IPv6> {
        size_t operator()(const libnet::IPv6& ip) const noexcept {
            return std::hash<libnet::uint128_t>{}(ip.GetRawAddress()) ^ 
                   (std::hash<uint16_t>{}(ip.GetPort()) << 1);
        }
    };
}

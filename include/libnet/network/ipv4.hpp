#pragma once

#include <ostream>
#include <cstdint>
#include <string>

#include <netinet/in.h>

namespace libnet {
    class IPv4 {
    private:
        uint32_t _addr {0};
        uint16_t _port {0};

    public:
        IPv4() = default;
        IPv4(const char* address);
        IPv4(const std::string_view addr) : IPv4(std::string(addr).c_str()) {};
        IPv4(const uint32_t address, const uint16_t port) noexcept
            : _addr(address), _port(port) {}

        IPv4(const IPv4&) = default;
        IPv4(IPv4&&) = default;
        ~IPv4() = default;

        IPv4(const sockaddr_in& sa) noexcept;

        IPv4& operator = (const char* address) {
            *this = IPv4(address);
            return *this;
        }
        IPv4& operator = (std::string_view addr) {
            *this = IPv4(addr);
            return *this;
        }

        IPv4& operator = (const IPv4&) = default;
        IPv4& operator = (IPv4&&) = default;

        bool operator == (const IPv4& other) const noexcept {
            return _addr == other._addr && _port == other._port;
        }

        std::string GetFullAddress() const noexcept;
        std::string GetAddress() const noexcept;
        uint16_t GetPort() const noexcept { return _port; }
        uint32_t GetRawAddress() const noexcept { return _addr; }

        sockaddr_in GetSockAddr() const noexcept;
    };

    inline std::ostream& operator << (std::ostream& os, const IPv4& ip) {
        os << ip.GetFullAddress();
        return os;
    }
}

namespace std {
    template<> struct hash<libnet::IPv4> {
        size_t operator()(const libnet::IPv4& ip) const noexcept {
            size_t h1 = std::hash<uint32_t>{}(ip.GetRawAddress());
            size_t h2 = std::hash<uint16_t>{}(ip.GetPort());
            return h1 ^ (h2 << 1);
        }
    };
}
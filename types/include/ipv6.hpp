#pragma once

#include <string>
#include <cstdint>

namespace libnet {

class IPv6 {
private:
    uint16_t _segment[8] = {0};
    uint16_t _port = 0;

public:
    IPv6() = default;
    IPv6(const char* address);
    IPv6(const std::string_view addr) : IPv6(std::string(addr).c_str()) {};
    IPv6(const uint16_t *address[], const uint16_t port) noexcept;

};



}
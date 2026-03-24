#pragma once

#include <string>
#include <cstdint>

namespace libnet {

class IPv6 {
private:
    uint16_t _segment[8] = {0};
    uint16_t _port = 0;

public:
    IPv6(const char* address);

};



}
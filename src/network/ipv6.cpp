#include <libnet/network/ipv6.hpp>

libnet::IPv6::IPv6(const char* address) {
    int double_colon_idx = -1;
    int current_segment = 0;
    uint32_t temp = 0;
    const char* p = address;
}
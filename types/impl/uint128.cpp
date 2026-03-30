#include "types/include/uint128.hpp"

std::string libnet::uint128_t::to_string() const noexcept {
    if(value == 0) return "0";

    std::string s;
    __uint128_t temp = value;
    while (temp > 0)
    {
        s += (char)('0' + (temp % 10));
        temp /= 10;
    }
    std::reverse(s.begin(), s.end());
    return s;
}
#include <libnet/core/uint128.hpp>


libnet::uint128_t::uint128_t(std::string_view sv) noexcept : value(0) {
    for(char c : sv) {
        if(c >= '0' && c <= '9') {
            value = (value * 10) + (c - '0');
        } else {
            break;
        }
    }
}

std::string libnet::uint128_t::to_string() const noexcept {
    if (value == 0) return "0";

    std::string s;
    s.reserve(40); // Максимальное число цифр в uint128 ~39
    __uint128_t temp = value;
    while (temp > 0) {
        s += (char)('0' + static_cast<uint64_t>(temp % 10));
        temp /= 10;
    }
    std::reverse(s.begin(), s.end());
    return s;
}
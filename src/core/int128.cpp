#include <libnet/core/int128.hpp>

libnet::int128_t::int128_t(std::string_view sv) noexcept : value(0) {
    if (sv.empty()) return;

    bool negative = false;
    size_t i = 0;
    if (sv[0] == '-') {
        negative = true;
        i = 1;
    }

    for (; i < sv.size(); ++i) {
        if (sv[i] >= '0' && sv[i] <= '9') {
            value = (value * 10) + (sv[i] - '0');
        } else break;
    }
    if (negative) value = -value;
}

std::string libnet::int128_t::to_string() const noexcept {
    if (value == 0) return "0";

    __int128 temp = value;
    std::string s;
    s.reserve(41); 
    
    bool negative = (temp < 0);

    while (temp != 0) {
        int digit = static_cast<int>(temp % 10);
        if (digit < 0) digit = -digit; // Берем модуль цифры
        s += static_cast<char>('0' + digit);
        temp /= 10;
    }

    if (negative) s += '-';
    std::reverse(s.begin(), s.end());
    return s;
}

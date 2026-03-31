#pragma once

#include <iostream>
#include <string>
#include <algorithm>
#include <cstdint>
#include <type_traits>

namespace libnet {

class int128_t {
private:
    __int128 value;

public:
    int128_t() noexcept : value(0) {}
    int128_t(__int128 v) noexcept : value(v) {}

    template<typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
    int128_t(T v) noexcept : value(static_cast<__int128>(v)) {}

    int128_t(int64_t high, uint64_t low) noexcept 
        : value((static_cast<__int128>(high) << 64) | low) {}

    explicit int128_t(std::string_view sv) noexcept;

    // Арифметика
    int128_t operator+(const int128_t& other) const noexcept { return value + other.value; }
    int128_t operator-(const int128_t& other) const noexcept { return value - other.value; }
    int128_t operator-() const noexcept { return -value; } // Унарный минус
    int128_t operator*(const int128_t& other) const noexcept { return value * other.value; }
    int128_t operator/(const int128_t& other) const noexcept { return value / other.value; }
    int128_t operator%(const int128_t& other) const noexcept { return value % other.value; }

    // Сравнения
    bool operator==(const int128_t& other) const noexcept { return value == other.value; }
    bool operator<(const int128_t& other) const noexcept { return value < other.value; }
    bool operator>(const int128_t& other) const noexcept { return value > other.value; }
    bool operator<=(const int128_t& other) const noexcept { return value <= other.value; }
    bool operator>=(const int128_t& other) const noexcept { return value >= other.value; }
    bool operator!=(const int128_t& other) const noexcept { return !(*this == other); }

    // Операторы присваивания
    int128_t& operator+=(const int128_t& other) noexcept { value += other.value; return *this; }
    int128_t& operator-=(const int128_t& other) noexcept { value -= other.value; return *this; }
    int128_t& operator*=(const int128_t& other) noexcept { value *= other.value; return *this; }
    int128_t& operator/=(const int128_t& other) noexcept { value /= other.value; return *this; }

    // Инкремент/Декремент
    int128_t& operator++()    { ++value; return *this; }
    int128_t  operator++(int) { int128_t tmp = *this; ++value; return tmp; }
    int128_t& operator--()    { --value; return *this; }
    int128_t  operator--(int) { int128_t tmp = *this; --value; return tmp; }

    // Битовые
    int128_t operator&(const int128_t& other) const noexcept { return value & other.value; }
    int128_t operator|(const int128_t& other) const noexcept { return value | other.value; }
    int128_t operator~() const noexcept { return {~value}; }

    int128_t operator<<(int shift) const noexcept { return {value << shift}; }
    int128_t operator>>(int shift) const noexcept { return {value >> shift}; }

    // Логические
    bool operator!() const noexcept { return !value; }
    explicit operator bool() const noexcept { return value != 0; }

    std::string to_string() const noexcept;
    int64_t high() const noexcept { return static_cast<int64_t>(value >> 64); }
    uint64_t low() const noexcept { return static_cast<uint64_t>(value); }
    __int128 raw() const noexcept { return value; }

    friend std::ostream& operator<<(std::ostream& os, const int128_t& obj) {
        return os << obj.to_string();
    }
};

} // namespace libnet

namespace std {
    template<>
    struct hash<libnet::int128_t> {
        size_t operator()(const libnet::int128_t& k) const noexcept {
            uint64_t h1 = static_cast<uint64_t>(k.high());
            uint64_t h2 = k.low();
            return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));
        }
    };
}

#pragma once

#include <iostream>
#include <string>
#include <algorithm>

#include <cstdint>



namespace libnet {

class uint128_t {
private:
    __uint128_t value;

    uint128_t(__uint128_t v) noexcept : value(v) {}

public:
    uint128_t() noexcept : value(0) {};


    // Шаблонный конструктор для ЛЮБЫХ целых чисел (кроме __uint128_t)
    template<typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
    uint128_t(T v) noexcept : value(static_cast<uint64_t>(v)) {}

    // Явный конструктор из двух половин
    uint128_t(uint64_t high, uint64_t low) noexcept
        : value((static_cast<__uint128_t>(high) << 64) | low) {}

    explicit uint128_t(std::string_view sv) noexcept;
    

    // --- Арифметика ---
    uint128_t operator+(const uint128_t& other) const noexcept { return {value + other.value}; }
    uint128_t operator-(const uint128_t& other) const noexcept { return {value - other.value}; }

    // --- Сравнения ---
    bool operator==(const uint128_t& other) const noexcept { return value == other.value; }
    bool operator==(uint64_t other) const noexcept { return value == other; }
    friend bool operator==(uint64_t lhs, const uint128_t& rhs) noexcept { return lhs == rhs.value; }

    bool operator!=(const uint128_t& other) const noexcept { return !(*this == other); }
    bool operator!=(uint64_t other) const noexcept { return !(*this == other); }

    bool operator<(const uint128_t& other) const noexcept { return value < other.value; }
    bool operator>(const uint128_t& other) const noexcept { return value > other.value; }

    // --- Битовые операции ---
    uint128_t operator&(const uint128_t& other) const noexcept { return {value & other.value}; }
    uint128_t operator|(const uint128_t& other) const noexcept { return {value | other.value}; }
    uint128_t operator^(const uint128_t& other) const noexcept { return {value ^ other.value}; }
    uint128_t operator~() const noexcept { return {~value}; }

    uint128_t operator<<(int shift) const noexcept { return {value << shift}; }
    uint128_t operator>>(int shift) const noexcept { return {value >> shift}; }

    // --- Вспомогательные функции ---
    std::string to_string() const noexcept;
    uint64_t high() const noexcept { return static_cast<uint64_t>(value >> 64); }
    uint64_t low() const noexcept { return static_cast<uint64_t>(value); }
    __uint128_t raw() const noexcept { return value; }

    friend std::ostream& operator<<(std::ostream& os, const uint128_t& obj) {
        return os << obj.to_string();
    }
};

}

namespace std {
    template<>
    struct hash<libnet::uint128_t> {
        size_t operator()(const libnet::uint128_t& k) const noexcept {
            // Комбинируем хеш от двух 64-битных половинок
            uint64_t h1 = k.high();
            uint64_t h2 = k.low();

            // Класический алгоритм комбинации хешей (как в Boost)
            return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));
        }
    };
}
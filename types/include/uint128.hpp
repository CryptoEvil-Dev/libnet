#pragma once

#include <iostream>
#include <string>
#include <algorithm>

#include <cstdint>



namespace libnet {

class uint128_t {
private:
    __uint128_t value;

public:
    ~uint128_t() = default;

    uint128_t(uint64_t high = 0, uint64_t low = 0) noexcept
        : value((static_cast<__uint128_t>(high) << 64) | low) {}

    uint128_t(const char* data) noexcept;


    uint128_t operator + (const uint128_t& other) const {
        uint128_t res;
        res.value = this->value + other.value;
        return res;
    }

    uint128_t operator - (const uint128_t& other) const {
        uint128_t res;
        res.value = this->value - other.value;
        return res;
    }

    uint128_t& operator = (const uint128_t& other) noexcept {
        this->value = other.value;
        return *this;
    }

    bool operator == (const uint128_t& other) const noexcept {
        return this->value == other.value;
    }

    bool operator != (const uint128_t& other) const noexcept {
        return !(*this == other);
    }

    bool operator < (const uint128_t& other) const noexcept {
        return this->value < other.value;
    }

    bool operator > (const uint128_t& other) const noexcept {
        return this->value > other.value;
    }

    uint128_t operator & (const uint128_t& other) const noexcept { return {value & other.value}; }
    uint128_t operator | (const uint128_t& other) const noexcept { return {value | other.value}; }
    uint128_t operator ^ (const uint128_t& other) const noexcept { return {value ^ other.value}; }
    uint128_t operator ~ () const noexcept { return {~value}; }

    uint128_t operator << (int shift) const noexcept { return {value << shift}; }
    uint128_t operator >> (int shift) const noexcept { return {value >> shift}; }




    std::string to_string() const noexcept;
    uint64_t high() const noexcept { return static_cast<uint64_t>(value >> 64); }
    uint64_t low() const noexcept { return static_cast<uint64_t>(value); }

};

}
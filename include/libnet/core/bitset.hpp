#pragma once
#include <vector>
#include <cstdint>
#include <optional>

#ifdef __GNUC__
    #define LIBNET_CTZLL __builtin_ctzll
#else
    // Простая реализация для других компиляторов
    inline int LIBNET_CTZLL(unsigned long long x) {
        int count = 0;
        while(!(x & 1)) { x >>= 1; count++; }
        return count;
    }
#endif

namespace libnet {

class bitset {
private:
    std::vector<uint64_t> _data;
    size_t _bits_count;

public:
    bitset(size_t bits) : _bits_count(bits) {
        _data.resize((bits + 63) / 64, 0);
    }
    void set(size_t bit_idx) noexcept;
    void reset(size_t bit_idx) noexcept;
    bool test(size_t bit_idx) const noexcept;
    std::optional<size_t> FindFirstFree() const;

};

}
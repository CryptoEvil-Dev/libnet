#include <libnet/core/bitset.hpp>

void libnet::bitset::set(size_t bit_idx) noexcept {
    if(bit_idx < _bits_count)
        _data[bit_idx / 64] |= (1ULL << (bit_idx % 64));
}

void libnet::bitset::reset(size_t bit_idx) noexcept  {
    if(bit_idx < _bits_count)
        _data[bit_idx / 64] &= ~(1ULL << (bit_idx % 64));
}

bool libnet::bitset::test(size_t bit_idx) const noexcept  {
    if (bit_idx >= _bits_count) return true;
        return _data[bit_idx / 64] & (1ULL << (bit_idx % 64));
}

std::optional<size_t> libnet::bitset::FindFirstFree() const {
    for (size_t i = 0; i < _data.size(); ++i) {
            if (_data[i] != ~0ULL) { // Если в слове есть хоть один ноль
                uint64_t inverted = ~_data[i];
                size_t bit_in_word = LIBNET_CTZLL(inverted);
                size_t res = i * 64 + bit_in_word;
                return (res < _bits_count) ? std::make_optional(res) : std::nullopt;
            }
        }
    return std::nullopt;
}
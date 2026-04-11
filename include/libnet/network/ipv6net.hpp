#pragma once
#include <libnet/network/ipv6.hpp>
#include <libnet/core/bitset.hpp>
#include <memory>
#include <string_view>
#include <mutex>

namespace libnet {

class IPv6Net {
private:
    libnet::IPv6 _base;
    uint8_t _prefix;
    std::unique_ptr<libnet::bitset> _pool;
    mutable std::mutex _mutex;
    static constexpr size_t MAX_POOL_SIZE = 65536; // Ограничиваем пул для экономии памяти

    uint128_t CalculateMask(uint8_t prefix) const noexcept;

public:
    IPv6Net(std::string_view cidr, size_t pool_size = MAX_POOL_SIZE);

    bool Contains(const libnet::IPv6& ip) const noexcept;
    bool IsFree(const libnet::IPv6& ip) const noexcept;

    libnet::IPv6 Allocate();
    void Release(const libnet::IPv6& ip) noexcept;
    void Reserve(const libnet::IPv6& ip);

    uint8_t GetPrefix() const noexcept { return _prefix; }
    libnet::IPv6 GetBaseAddress() const noexcept { return _base; }
};

}

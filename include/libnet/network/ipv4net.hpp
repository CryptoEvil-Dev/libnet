#pragma once
#include <libnet/network/ipv4.hpp>
#include <libnet/core/bitset.hpp>
#include <memory>
#include <string_view>
#include <mutex>

namespace libnet {

class IPv4Net {
private:
    libnet::IPv4 _base;
    uint8_t _prefix;
    std::unique_ptr<libnet::bitset> _pool;
    mutable std::mutex _mutex;

public:
    IPv4Net(std::string_view cidr);

    // Проверка вхождения в сеть (без учета занятости)
    bool Contains(const libnet::IPv4& ip) const noexcept;
    
    // Проверка: свободен ли адрес для выделения
    bool IsFree(const libnet::IPv4& ip) const noexcept;

    // Выделение и освобождение
    libnet::IPv4 Allocate();
    void Release(const IPv4& ip) noexcept;
    void Reserve(const libnet::IPv4& ip);

    uint8_t GetPrefix() const noexcept { return _prefix; }
    libnet::IPv4 GetBaseAddress() const noexcept { return _base; }
};

}

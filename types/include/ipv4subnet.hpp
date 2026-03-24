// Простой набросок! Переписать и дополнить в будущем!
// ВОЗМОЖНО ОПАСНЫЙ КОД!
#pragma once

#include <cstdint>
#include "types/include/ipv4.hpp"

namespace libnet {

class IPv4Subnet {
private:
    uint32_t _base_addr; // Начальный адрес (Например: 10.0.0.0)
    uint32_t _mask;      // Маска (Например 24)
    uint32_t _current;   // Смещение для следующего адреса

public:
    IPv4Subnet(uint32_t base, uint32_t mask_bits)
        : _base_addr(base), _mask(mask_bits), _current(1) {}; // Начинаем с .1
    

    // Сколько всего хостов доступно в этой маске
    size_t capacity() const {
        return 1ULL << (32 - _mask);
    }

    // Проверка: Входит ли IP в эту подсеть
    // bool contains(libnet::IPv4 addr) const {
        
    // }

    libnet::IPv4 next() {
        // Простая проверка: Не вышли ли мы за пределы маски (например, /24 это 254 адреса)
        uint32_t max_hosts = (1 << (32 - _mask)) - 2;
        if(_current > max_hosts) {
            throw std::runtime_error("Subnet is full");
        }

        // Собираем IP: база + смещение
        return libnet::IPv4(_base_addr | _current++, 0);
    }
};

}
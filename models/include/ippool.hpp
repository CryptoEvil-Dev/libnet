#pragma once

#include "types/types.hpp"
#include <unordered_map>
#include <chrono>


namespace libnet {

struct PeerInfo {
    libnet::IPv4 v_addr; // Виртуальный IP
    libnet::IPv4 r_addr; // Реальный IP
    std::chrono::steady_clock::time_point rent_time; // Время аренды
};

class IPPool {
private:
    std::unordered_map<libnet::IPv4, libnet::PeerInfo> peers;

public:
    IPPool() = default;

    libnet::PeerInfo acquire(const libnet::IPv4 peer); // выделяет свободный IP для нового пира и фиксирует аренду.
    void release(const libnet::IPv4 peer) noexcept; // принудительно освобождает адрес (виртуальный или реальный).
    libnet::PeerInfo refresh(const libnet::IPv4 peer) noexcept; // обновляет TTL (время жизни) аренды при получении пакета от пира.
    libnet::PeerInfo lookup(const libnet::IPv4 peer) noexcept; // быстро находит реальный адрес для инкапсуляции пакета (самый часто вызываемый метод).

};



}
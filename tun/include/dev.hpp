#pragma once

#include "types/include/ipv4.hpp"
#include "models/include/ippool.hpp"

#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <net/if.h>


#include <string>

namespace libnet {

struct TunConfig {
    // Системные дескрипторы
    int tun_fd;         // Файловый дескриптор устройства /dev/net/tun
    int net_socket;     // Сокет для передачи инкапсулированных данных (UDP/TCP)

    // Параметры интерфейса
    std::string dev_name; // Имя интерфейса
    int mtu;              // Максимальный размер пакета (обычно 1500)

    // Адресация
    libnet::IPv4 v_addr; // IP-адрес, который будет назначен TUN интерфейсу
    libnet::IPv4 r_addr; // IP-адрес удалённой стороны (куда шлём пакеты)

    // Флаги (IFF_TUN, IFF_TAP, IFF_NO_PI)
    short flags;
};

class TUN {
private:
    libnet::TunConfig _info;
    libnet::IPPool _pool;

public:
    TUN() = delete;
    TUN(const TUN&) = delete;
    TUN(TUN&&) = delete;

    TUN(std::string dev_name, libnet::IPv4 addr);
    TUN(libnet::TunConfig info);

};

}
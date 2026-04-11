#include <libnet/dev/tun.hpp>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/if.h>
#include <linux/if_tun.h>
#include <unistd.h>
#include <cstring>


libnet::TUN::TUN(const std::string& name) {
    _fd = open("/dev/net/tun", O_RDWR);
    if(_fd < 0) throw std::runtime_error("failed to open /dev/net/tun (try sudo)");

    struct ifreq ifr;
    std::memset(&ifr, 0, sizeof(ifr));

    // IFF_TUN - работаем с пакетами (Layer 3)
    // IFF_NO_PI - не добавлять лишние 4 байта заголовка (Packet Info)
    ifr.ifr_flags = IFF_TUN | IFF_NO_PI;

    if(!name.empty()) {
        std::strncpy(ifr.ifr_name, name.c_str(), IFNAMSIZ);
    }

    if(ioctl(_fd, TUNSETIFF, (void*)&ifr) < 0){
        ::close(_fd);
        throw std::runtime_error("ioctl(TUNSETIFF) failed");
    }

    _ifname = ifr.ifr_name;

};

libnet::TUN::~TUN() {
    if(_fd >= 0) ::close(_fd);
}


libnet::TUN::TUN(TUN&& other) noexcept : _fd(other._fd), _ifname(std::move(other._ifname)) {
    other._fd = -1;
}

libnet::TUN& libnet::TUN::operator=(TUN&& other) noexcept {
    if(this != &other) {
        if(_fd >= 0) ::close(_fd);
        _fd = other._fd;
        _ifname = std::move(other._ifname);
        other._fd = -1;
    }
    return *this;
}


void libnet::TUN::Configure(const libnet::IPv4& addr, uint8_t prefix, int mtu) {
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) throw std::runtime_error("Socket for ioctl failed");

    struct ifreq ifr;
    std::memset(&ifr, 0, sizeof(ifr));
    std::strncpy(ifr.ifr_name, _ifname.c_str(), IFNAMSIZ);

    // 1. Установка IP
    sockaddr_in* sin = (sockaddr_in*)&ifr.ifr_addr;
    sin->sin_family = AF_INET;
    sin->sin_addr.s_addr = htonl(addr.GetRawAddress());
    if (ioctl(sock, SIOCSIFADDR, &ifr) < 0) throw std::runtime_error("Failed SIOCSIFADDR");

    // 2. Установка маски подсети (напр. /24 -> 255.255.255.0)
    uint32_t mask = (prefix == 0) ? 0 : (~0U << (32 - prefix));
    sin->sin_addr.s_addr = htonl(mask);
    if (ioctl(sock, SIOCSIFNETMASK, &ifr) < 0) throw std::runtime_error("Failed SIOCSIFNETMASK");

    // 3. Установка MTU
    ifr.ifr_mtu = mtu;
    if (ioctl(sock, SIOCSIFMTU, &ifr) < 0) throw std::runtime_error("Failed SIOCSIFMTU");

    // 4. Поднятие интерфейса
    if (ioctl(sock, SIOCGIFFLAGS, &ifr) >= 0) {
        ifr.ifr_flags |= (IFF_UP | IFF_RUNNING);
        ioctl(sock, SIOCSIFFLAGS, &ifr);
    }

    ::close(sock);
}

ssize_t libnet::TUN::write(const void* data, size_t len) {
    if(_fd < 0) return -1;
    // Пишем сырой IP-пакет в дескриптор.
    // Ядро примет его и направит в локальный стек протоколов.
    return ::write(_fd, data, len);
}

std::vector<uint8_t> libnet::TUN::read(size_t max_len) {
    std::vector<uint8_t> buffer(max_len);
    if (_fd < 0) return {};

    // Блокирующее чтение пакета, пришедшего из системы.
    ssize_t n = ::read(_fd, buffer.data(), max_len);
    if(n < 0) {
        throw std::runtime_error("read from TUN failed");
    }

    buffer.resize(n);
    return buffer;
}
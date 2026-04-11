#include <libnet/dev/udpsocket.hpp>
#include <sys/socket.h>
#include <unistd.h>
#include <stdexcept>
#include <cstring>

libnet::UDPSocket::UDPSocket(const libnet::IPv4& addr) : _base(addr) {
    _fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(_fd < 0) throw std::runtime_error("Failed to create IPv4 socket");

    auto sa = addr.GetSockAddr();
    if(bind(_fd, (struct sockaddr*)&sa, sizeof(sa)) < 0) {
        close(_fd);
        throw std::runtime_error("Failed to bind IPv4 socket");
    }
};

libnet::UDPSocket::UDPSocket(const libnet::IPv6& addr) : _base(addr) {
    _fd = socket(AF_INET6, SOCK_DGRAM, 0);
    if(_fd < 0) throw std::runtime_error("Failed to create IPv6 socket");

    auto sa = addr.GetSockAddr();
    if(bind(_fd, (struct sockaddr*)&sa, sizeof(sa)) < 0) {
        close(_fd);
        throw std::runtime_error("Failed to bind IPv6 socket");
    }
};

libnet::UDPSocket::~UDPSocket() {
    if(_fd >= 0) close(_fd);
};

libnet::UDPSocket& libnet::UDPSocket::operator=(libnet::UDPSocket&& other) noexcept {
    if(this != &other) {
        if(_fd >= 0) close(_fd);
        _fd = other._fd;
        _base = std::move(other._base);
        other._fd = -1;
    }
    return *this;
};

ssize_t libnet::UDPSocket::sendto(const libnet::IPv4& dest, const void* data, size_t len) {
    auto sa = dest.GetSockAddr();
    return ::sendto(_fd, data, len, 0, (struct sockaddr*)&sa, sizeof(sa));
};

ssize_t libnet::UDPSocket::sendto(const libnet::IPv6& dest, const void* data, size_t len) {
    auto sa = dest.GetSockAddr();
    return ::sendto(_fd, data, len, 0, (struct sockaddr*)&sa, sizeof(sa));
};

ssize_t libnet::UDPSocket::sendto(const libnet::UDPdata& packet) {
    return std::visit([&](auto&& addr) {
        return this->sendto(addr, packet.data.data(), packet.data.size());
    }, packet.addr);
};

libnet::UDPdata libnet::UDPSocket::recv(size_t max_len) {
    std::vector<uint8_t> buffer(max_len);

    sockaddr_storage src_addr{};
    socklen_t addr_len = sizeof(src_addr);

    ssize_t n = recvfrom(_fd, buffer.data(), max_len, 0, (struct sockaddr*)&src_addr, &addr_len);
    if(n < 0) throw std::runtime_error("recvfrom failed");

    buffer.resize(n);

    if(src_addr.ss_family == AF_INET) {
        return { libnet::IPv4(*reinterpret_cast<sockaddr_in*>(&src_addr)), std::move(buffer) };
    } else {
        return { libnet::IPv6(*reinterpret_cast<sockaddr_in6*>(&src_addr)), std::move(buffer) };
    }
}
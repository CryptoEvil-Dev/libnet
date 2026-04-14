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

std::pair<libnet::IPv4, std::vector<uint8_t>> libnet::UDPSocket::recv(size_t max_len) {
    std::vector<uint8_t> buffer(max_len);
    sockaddr_in src_addr{};
    socklen_t addr_len = sizeof(src_addr);

    size_t n = ::recvfrom(_fd, buffer.data(), max_len, 0, (struct sockaddr*)&src_addr, &addr_len);
    if(n < 0) throw std::runtime_error("UDP recv failed");

    buffer.resize(n);
    return { libnet::IPv4(src_addr), std::move(buffer) };
}

std::pair<libnet::IPv4, libnet::IPPacket> libnet::UDPSocket::recv_packet(size_t max_len) {
    auto [from, data] = recv(max_len);
    return {from, libnet::IPPacket(std::move(data))};
}

ssize_t libnet::UDPSocket::sendto(const libnet::IPv4& dest, const libnet::IPPacket& pkt) {
    auto sa = dest.GetSockAddr();
    return ::sendto(_fd, pkt.ToVector().data(), pkt.Size(), 0, (struct sockaddr*)&sa, sizeof(sa));
}
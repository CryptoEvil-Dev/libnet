#pragma once

#include <libnet/network/ipv4.hpp>
#include <libnet/network/ipv6.hpp>
#include <libnet/network/packet.hpp>
#include <vector>
#include <variant>
#include <utility>

namespace libnet {

class UDPSocket {
private:
    int _fd;
    std::variant<libnet::IPv4, libnet::IPv6> _base;

public:
    UDPSocket(const libnet::IPv4& addr);
    UDPSocket(const libnet::IPv6& addr);
    ~UDPSocket();

    UDPSocket(const UDPSocket&) = delete;
    UDPSocket& operator = (const UDPSocket&) = delete;

    UDPSocket(UDPSocket&&) noexcept;
    UDPSocket& operator = (UDPSocket&&) noexcept;

    // Отправка данных
    ssize_t sendto(const libnet::IPv4& dest, const void* data, size_t len);
    ssize_t sendto(const libnet::IPv6& dest, const void* data, size_t len);
    ssize_t sendto(const libnet::IPv4& dest, const libnet::IPPacket& pkt);

    std::pair<libnet::IPv4, std::vector<uint8_t>> recv(size_t max_len = 4096);
    std::pair<libnet::IPv4, libnet::IPPacket> recv_packet(size_t max_len = 4096);


    
    // Вернуть дескриптор
    int get_fd() const noexcept { return _fd; }
};

}
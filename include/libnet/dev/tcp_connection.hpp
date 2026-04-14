#pragma once
#include <libnet/network/ipv4.hpp>
#include <vector>
#include <memory>

namespace libnet {

class TCPConnection {
private:
    int _fd;
    libnet::IPv4 _addr;
    bool _isConnected {false};

public:
    TCPConnection();
    TCPConnection(int fd, const IPv4& addr);
    ~TCPConnection();

    TCPConnection(const TCPConnection&) = delete;
    TCPConnection& operator = (const TCPConnection&) = delete;

    TCPConnection(TCPConnection&&) noexcept;
    TCPConnection& operator = (TCPConnection&&) noexcept;

    void connect(const IPv4& addr);
    ssize_t send(const void* data, size_t len);

    // Важно! если вернётся пустой вектор - значит соединение закрыто!
    std::vector<uint8_t> recv(size_t max_len = 4096);

    void close();

    int get_fd() const noexcept { return _fd; }
    const IPv4& get_remote_addr() const noexcept { return _addr; }
    bool is_active() const noexcept { return _isConnected; }

    // Системный Keep-Alive (через setsockopt)
    void SetKeepAlive(bool enabled, int idle_sec = 60, int interval_sec = 5, int count = 3);


};

}
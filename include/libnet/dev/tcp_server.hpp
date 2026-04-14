#pragma once
#include <libnet/dev/tcp_connection.hpp>
#include <memory>

namespace libnet  {

class TCPServer {
private:
    int _listen_fd;
    libnet::IPv4 _addr;

public:
    TCPServer(const libnet::IPv4& addr, int backlog = 128);
    ~TCPServer();

    TCPServer(const TCPServer&) = delete;
    TCPServer& operator = (const TCPServer&) = delete;

    // Принимает новое соединение. В неблокирующем режиме вернёт nullptr, если никого нет.
    std::unique_ptr<libnet::TCPConnection> accept();

    int get_fd() const noexcept { return _listen_fd; }
    const libnet::IPv4& get_addr() const noexcept { return _addr; }

    void close();
};

}
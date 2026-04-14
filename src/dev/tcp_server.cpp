#include <libnet/dev/tcp_server.hpp>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdexcept>
#include <cstring>

libnet::TCPServer::TCPServer(const libnet::IPv4& addr, int backlog) : _addr(addr) {
    _listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(_listen_fd < 0) throw std::runtime_error("failed to create listen socket");

    int opt = 1;
    setsockopt(_listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    auto sa = addr.GetSockAddr();
    if(bind(_listen_fd, (struct sockaddr*)&sa, sizeof(sa)) < 0) {
        ::close(_listen_fd);
        throw std::runtime_error("bind failed" + std::string(strerror(errno)));
    }

    if(listen(_listen_fd, backlog) < 0) {
        ::close(_listen_fd);
        throw std::runtime_error("listen failed");
    }
}

libnet::TCPServer::~TCPServer() {
    close();
}

std::unique_ptr<libnet::TCPConnection> libnet::TCPServer::accept() {
    sockaddr_in client_addr{};
    socklen_t addr_len = sizeof(client_addr);

    int client_fd = ::accept(_listen_fd, (struct sockaddr*)&client_addr, &addr_len);

    if(client_fd < 0) {
        if(errno == EAGAIN || errno == EWOULDBLOCK) return nullptr;
        return nullptr;
    }

    return std::make_unique<libnet::TCPConnection>(client_fd, libnet::IPv4(client_addr));
}

void libnet::TCPServer::close() {
    if(_listen_fd >= 0) {
        ::close(_listen_fd);
        _listen_fd = -1;
    }
}
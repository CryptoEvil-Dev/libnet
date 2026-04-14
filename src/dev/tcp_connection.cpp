#include <libnet/dev/tcp_connection.hpp>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdexcept>
#include <cstring>

libnet::TCPConnection::TCPConnection() {
    _fd = socket(AF_INET, SOCK_STREAM, 0);
    if(_fd < 0) throw std::runtime_error("failed to create TCP socket");
}

libnet::TCPConnection::TCPConnection(int fd, const libnet::IPv4& addr) : _fd(fd), _addr(addr), _isConnected(true) {}

libnet::TCPConnection::~TCPConnection() {
    close();
}

libnet::TCPConnection::TCPConnection(libnet::TCPConnection&& other) noexcept 
    : _fd(other._fd), _addr(other._addr), _isConnected(other._isConnected) {
    other._fd = -1;
    other._isConnected = false;
}

libnet::TCPConnection& libnet::TCPConnection::operator=(TCPConnection&& other) noexcept {
    if (this != &other) {
        close();
        _fd = other._fd;
        _addr = other._addr;
        _isConnected = other._isConnected;
        other._fd = -1;
        other._isConnected = false;
    }
    return *this;
}

void libnet::TCPConnection::connect(const IPv4& addr) {
    auto sa = addr.GetSockAddr();
    if (::connect(_fd, (struct sockaddr*)&sa, sizeof(sa)) < 0) {
        throw std::runtime_error("Connect failed: " + std::string(strerror(errno)));
    }
    _addr = addr;
    _isConnected = true;
}

ssize_t libnet::TCPConnection::send(const void* data, size_t len) {
    if (!_isConnected) return -1;
    
    size_t total_sent = 0;
    const uint8_t* buf = reinterpret_cast<const uint8_t*>(data);

    while (total_sent < len) {
        ssize_t n = ::send(_fd, buf + total_sent, len - total_sent, MSG_NOSIGNAL);
        
        if (n > 0) {
            total_sent += n;
        } else if (n < 0) {
            // Если сокет неблокирующий и буфер полон — нужно подождать.
            // Но для базовой реализации мы можем просто вернуть ошибку 
            // или попробовать снова, если это EINTR.
            if (errno == EINTR) continue;
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                // В идеале тут нужно подождать через poll/select, 
                // но для начала просто вернем сколько успели отправить.
                return total_sent; 
            }
            _isConnected = false;
            return -1;
        } else {
            // n == 0
            break;
        }
    }
    return total_sent;
}

std::vector<uint8_t> libnet::TCPConnection::recv(size_t max_len) {
    std::vector<uint8_t> buffer(max_len);
    ssize_t n = ::recv(_fd, buffer.data(), max_len, 0);

    if (n > 0) {
        buffer.resize(n);
        return buffer;
    } else if (n == 0) {
        // Соединение закрыто другой стороной
        _isConnected = false;
        return {};
    } else {
        // Ошибка или EAGAIN (если сокет неблокирующий)
        if (errno == EAGAIN || errno == EWOULDBLOCK) return {};
        _isConnected = false;
        return {};
    }
}

void libnet::TCPConnection::close() {
    if (_fd >= 0) {
        ::close(_fd);
        _fd = -1;
        _isConnected = false;
    }
}

void libnet::TCPConnection::SetKeepAlive(bool enable, int idle_sec, int interval_sec, int count) {
    int opt = enable ? 1 : 0;
    setsockopt(_fd, SOL_SOCKET, SO_KEEPALIVE, &opt, sizeof(opt));
    
    if (enable) {
        setsockopt(_fd, IPPROTO_TCP, TCP_KEEPIDLE, &idle_sec, sizeof(idle_sec));
        setsockopt(_fd, IPPROTO_TCP, TCP_KEEPINTVL, &interval_sec, sizeof(interval_sec));
        setsockopt(_fd, IPPROTO_TCP, TCP_KEEPCNT, &count, sizeof(count));
    }
}

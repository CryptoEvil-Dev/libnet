#include <libnet/core/eloop.hpp>
#include <unistd.h>
#include <fcntl.h>
#include <stdexcept>
#include <cstring>

void libnet::EventLoop::set_non_blocking(int  fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

libnet::EventLoop::EventLoop() {
    _epoll_fd = epoll_create1(EPOLL_CLOEXEC);
    if(_epoll_fd < 0) throw std::runtime_error("failed to create epoll");
}

libnet::EventLoop::~EventLoop() {
    Stop();
    if(_epoll_fd >= 0) ::close(_epoll_fd);
}

void libnet::EventLoop::Add(int fd, Handler callback) {
    // Делаем дескриптор неблокирующим
    int flags = fcntl(fd, F_GETFL, 0);
    if(flags == -1) throw std::runtime_error("fcntl GETFL failed");
    if(fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) throw std::runtime_error("fcntl SETFL O_NONBLOCK failed");

    struct epoll_event ev{};
    ev.events = EPOLLIN; // EPOLLIN - чтение
    ev.data.fd = fd;

    if(epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, fd, &ev) < 0) {
        throw std::runtime_error("epoll_ctl ADD failed:" + std::string(strerror(errno)));
    }
    _handlers[fd] = std::move(callback);
}

void libnet::EventLoop::Remove(int fd) {
    if(_handlers.find(fd) != _handlers.end()) {
        epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, fd, nullptr);
        _handlers.erase(fd);
    }
}

void libnet::EventLoop::Run() {
    _running = true;
    constexpr int MAX_EVENTS = 16;
    struct epoll_event events[MAX_EVENTS];

    while (_running) {
        // Таймаут 100мс, чтобы цикл мог периодически проверять флаг _running
        int nfds = epoll_wait(_epoll_fd, events, MAX_EVENTS, 100);
        
        if (nfds < 0) {
            if (errno == EINTR) continue; // Перехват системного прерывания
            break;
        }

        for (int n = 0; n < nfds; ++n) {
            int fd = events[n].data.fd;
            auto it = _handlers.find(fd);
            if (it != _handlers.end()) {
                // Выполняем логику (пересылку пакета)
                it->second(fd);
            }
        }
    }
}

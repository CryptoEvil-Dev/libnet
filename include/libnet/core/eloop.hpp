#pragma once

#include <sys/epoll.h>
#include <functional>
#include <unordered_map>
#include <atomic>


namespace libnet {

class EventLoop {
public:
    using Handler = std::function<void(int fd)>;

private:
    int _epoll_fd;
    std::unordered_map<int, Handler> _handlers;
    std::atomic<bool> _running {false};

    void set_non_blocking(int fd);

public:
    EventLoop();
    ~EventLoop();

    EventLoop(const EventLoop&) = delete;
    EventLoop& operator = (const EventLoop&) = delete;

    // Добавить дескриптор в цикл
    void Add(int fd, Handler callback);
    // Удалить дескриптор из цикла
    void Remove(int fd);
    // Запуск цикла
    void Run();

    // Остановка
    void Stop() { _running = false; }


};

}
#include <gtest/gtest.h>
#include <libnet/core/eloop.hpp>
#include <libnet/dev/udpsocket.hpp>
#include <thread>

TEST(EventLoopTest, UDPBridge) {
    libnet::EventLoop loop;
    
    // Создаем два сокета на разных портах
    libnet::UDPSocket sock1(libnet::IPv4("127.0.0.1:0"));
    libnet::UDPSocket sock2(libnet::IPv4("127.0.0.1:0"));

    // Узнаем адрес второго сокета, чтобы знать, куда слать
    sockaddr_in sin;
    socklen_t len = sizeof(sin);
    getsockname(sock2.get_fd(), (struct sockaddr *)&sin, &len);
    libnet::IPv4 addr2(ntohl(sin.sin_addr.s_addr), ntohs(sin.sin_port));

    bool received = false;
    std::string payload = "PingPong";

    // 1. Когда данные приходят на первый сокет — пересылаем их второму
    loop.Add(sock1.get_fd(), [&](int fd) {
        auto msg = sock1.recv();
        sock1.sendto(addr2, msg.data.data(), msg.data.size());
    });

    // 2. Когда данные приходят на второй сокет — проверяем и стопаем цикл
    loop.Add(sock2.get_fd(), [&](int fd) {
        auto msg = sock2.recv();
        std::string s(msg.data.begin(), msg.data.end());
        if (s == payload) {
            received = true;
            loop.Stop();
        }
    });

    // 3. Поток-"пинатель": шлет пакет на ПЕРВЫЙ сокет
    std::thread sender([&]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        // Создаем временный сокет, чтобы просто "пульнуть" пакет
        libnet::UDPSocket client(libnet::IPv4("127.0.0.1:0"));
        
        sockaddr_in sin1;
        socklen_t len1 = sizeof(sin1);
        getsockname(sock1.get_fd(), (struct sockaddr *)&sin1, &len1);
        libnet::IPv4 addr1(ntohl(sin1.sin_addr.s_addr), ntohs(sin1.sin_port));
        
        client.sendto(addr1, payload.c_str(), payload.size());
    });

    // Запускаем цикл (таймаут 2 секунды на случай провала, чтобы не висеть вечно)
    // Для этого можно запустить loop в отдельном потоке или просто ждать.
    // Но так как у нас есть таймаут в epoll_wait (100ms), Run() будет проверять _running.
    loop.Run();

    if (sender.joinable()) sender.join();

    EXPECT_TRUE(received);
}

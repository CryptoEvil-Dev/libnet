#include <gtest/gtest.h>
#include <libnet/dev/tcp_server.hpp>
#include <libnet/dev/tcp_connection.hpp>
#include <libnet/core/eloop.hpp>
#include <thread>
#include <chrono>

TEST(TCPTest, EchoServer) {
    libnet::EventLoop loop;
    libnet::IPv4 server_addr("127.0.0.1:0"); // Случайный порт на loopback
    
    // 1. Создаем сервер
    libnet::TCPServer server(server_addr);
    
    // Выясняем реальный порт, который дала ОС
    sockaddr_in sin;
    socklen_t len = sizeof(sin);
    getsockname(server.get_fd(), (struct sockaddr *)&sin, &len);
    libnet::IPv4 real_addr(ntohl(sin.sin_addr.s_addr), ntohs(sin.sin_port));

    std::string secret_payload = "TCP_Power_Test";
    bool server_received = false;
    bool client_received = false;

    // 2. Логика сервера: принимаем соединение и ждем данные
    loop.Add(server.get_fd(), [&](int fd) {
        auto client_conn = server.accept();
        if (client_conn) {
            // Как только приняли клиента, подписываем его на события чтения
            // Используем shared_ptr, чтобы сохранить объект внутри лямбды
            auto shared_conn = std::shared_ptr<libnet::TCPConnection>(std::move(client_conn));
            
            loop.Add(shared_conn->get_fd(), [shared_conn, &loop, &server_received, &secret_payload](int fd) {
                auto data = shared_conn->recv();
                if (!data.empty()) {
                    std::string msg(data.begin(), data.end());
                    if (msg == secret_payload) {
                        server_received = true;
                        // Эхо-ответ клиенту
                        shared_conn->send(msg.data(), msg.size());
                    }
                } else {
                    // Клиент закрыл соединение
                    loop.Remove(fd);
                }
            });
        }
    });

    // 3. Логика клиента: подключаемся и ждем эхо
    libnet::TCPConnection client;
    std::thread client_thread([&]() {
        // Даем серверу время прогрузиться в EventLoop
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        
        client.connect(real_addr);
        client.send(secret_payload.data(), secret_payload.size());

        // Читаем ответ (блокирующее чтение для простоты в этом потоке)
        auto resp = client.recv();
        if (!resp.empty()) {
            std::string msg(resp.begin(), resp.end());
            if (msg == secret_payload) {
                client_received = true;
            }
        }
        
        // Останавливаем цикл, чтобы завершить тест
        loop.Stop();
    });

    // Запускаем реактор событий
    loop.Run();

    if (client_thread.joinable()) client_thread.join();

    EXPECT_TRUE(server_received);
    EXPECT_TRUE(client_received);
}

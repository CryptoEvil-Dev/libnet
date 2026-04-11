#include <gtest/gtest.h>
#include <libnet/dev/udpsocket.hpp>
#include <libnet/network/ipv4.hpp>
#include <libnet/network/ipv6.hpp>
#include <thread>
#include <chrono>

TEST(UDPSocketTest, SendAndReceiveIPv4) {
    // 1. Создаем два сокета на loopback-интерфейсе
    // Порт 0 позволяет ОС самой выбрать свободный порт
    libnet::UDPSocket server(libnet::IPv4("127.0.0.1:0"));
    libnet::UDPSocket client(libnet::IPv4("127.0.0.1:0"));

    // Выясняем, какой порт выделила ОС для сервера
    sockaddr_in sin;
    socklen_t len = sizeof(sin);
    getsockname(server.get_fd(), (struct sockaddr *)&sin, &len);
    libnet::IPv4 server_addr(ntohl(sin.sin_addr.s_addr), ntohs(sin.sin_port));

    std::string message = "Hello, Libnet!";
    
    // 2. Отправляем данные от клиента серверу
    client.sendto(server_addr, message.c_str(), message.size());

    // 3. Получаем данные на сервере
    auto result = server.recv();

    // 4. Проверки
    EXPECT_EQ(result.data.size(), message.size());
    std::string received_msg(result.data.begin(), result.data.end());
    EXPECT_EQ(received_msg, message);

    // Проверяем, что адрес отправителя (клиента) определился корректно
    EXPECT_TRUE(std::holds_alternative<libnet::IPv4>(result.addr));
}

TEST(UDPSocketTest, VariantDataInterface) {
    libnet::UDPSocket server(libnet::IPv4("127.0.0.1:0"));
    libnet::UDPSocket client(libnet::IPv4("127.0.0.1:0"));

    // Узнаем адрес сервера
    sockaddr_in sin;
    socklen_t len = sizeof(sin);
    getsockname(server.get_fd(), (struct sockaddr *)&sin, &len);
    libnet::IPv4 server_addr(ntohl(sin.sin_addr.s_addr), ntohs(sin.sin_port));

    // Готовим UDPdata
    libnet::UDPdata packet;
    packet.addr = server_addr;
    packet.data = { 'P', 'I', 'N', 'G' };

    // Отправляем через variant-интерфейс
    client.sendto(packet);

    auto result = server.recv();
    EXPECT_EQ(result.data[0], 'P');
}

TEST(UDPSocketTest, IPv6SimpleInit) {
    // Просто проверяем, что IPv6 сокет создается и биндится без ошибок
    // (на некоторых системах IPv6 может быть отключен в loopback, поэтому оборачиваем в try)
    try {
        libnet::UDPSocket sock_v6(libnet::IPv6("[::1]:0"));
        EXPECT_GT(sock_v6.get_fd(), 0);
    } catch (const std::exception& e) {
        std::cout << "[ SKIP ] IPv6 loopback not supported: " << e.what() << std::endl;
    }
}
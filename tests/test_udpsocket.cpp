#include <gtest/gtest.h>
#include <libnet/dev/udpsocket.hpp>
#include <libnet/network/ipv4.hpp>
#include <libnet/network/ipv6.hpp>
#include <libnet/network/packet.hpp> // Добавили для IPPacket
#include <thread>
#include <chrono>

TEST(UDPSocketTest, SendAndReceiveIPv4) {
    libnet::UDPSocket server(libnet::IPv4("127.0.0.1:0"));
    libnet::UDPSocket client(libnet::IPv4("127.0.0.1:0"));

    sockaddr_in sin;
    socklen_t len = sizeof(sin);
    getsockname(server.get_fd(), (struct sockaddr *)&sin, &len);
    libnet::IPv4 server_addr(ntohl(sin.sin_addr.s_addr), ntohs(sin.sin_port));

    std::string message = "Hello, Libnet!";
    client.sendto(server_addr, message.c_str(), message.size());

    // Теперь это std::pair<std::variant<IPv4, IPv6>, std::vector<uint8_t>>
    auto result = server.recv();

    // result.first — адрес, result.second — данные
    EXPECT_EQ(result.second.size(), message.size());
    std::string received_msg(result.second.begin(), result.second.end());
    EXPECT_EQ(received_msg, message);

    EXPECT_EQ(result.first.GetAddress(), "127.0.0.1");
}

TEST(UDPSocketTest, IPPacketInterface) {
    libnet::UDPSocket server(libnet::IPv4("127.0.0.1:0"));
    libnet::UDPSocket client(libnet::IPv4("127.0.0.1:0"));

    sockaddr_in sin;
    socklen_t len = sizeof(sin);
    getsockname(server.get_fd(), (struct sockaddr *)&sin, &len);
    libnet::IPv4 server_addr(ntohl(sin.sin_addr.s_addr), ntohs(sin.sin_port));

    // Вместо VariantDataInterface тестируем отправку нашего нового IPPacket
    libnet::IPv4 src("10.0.0.1:0");
    libnet::IPPacket packet(libnet::UDP, src, server_addr, "PING");

    client.sendto(server_addr, packet);

    auto result = server.recv();
    // Проверяем, что первый байт полезной нагрузки — 'E' (0x45 в IPv4 заголовке)
    // Так как сокет получил сырой IPPacket целиком
    EXPECT_EQ(result.second[0], 0x45); 
}

TEST(UDPSocketTest, IPv6SimpleInit) {
    try {
        libnet::UDPSocket sock_v6(libnet::IPv6("[::1]:0"));
        EXPECT_GT(sock_v6.get_fd(), 0);
    } catch (const std::exception& e) {
        std::cout << "[ SKIP ] IPv6 loopback not supported: " << e.what() << std::endl;
    }
}
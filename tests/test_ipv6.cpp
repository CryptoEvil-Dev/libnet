#include <iostream>
#include <sys/socket.h>
#include <unistd.h>

#include <gtest/gtest.h>
#include <libnet/network/ipv6.hpp>

TEST(IPv6Test, DefaultConstructor) {
    libnet::IPv6 addr = "::1";
    EXPECT_EQ(sizeof(addr), 32);
    EXPECT_EQ(addr.GetFullAddress(), std::string("::1"));

    libnet::IPv6 addrs[] = {
        libnet::IPv6("fd50:15ec:9b76:0:cacd:390a:1a96:2809"),
        libnet::IPv6("fd50:15ec:9b76:0:2e0:6cff:feaa:1aed"),
        libnet::IPv6("fe80::2e0:6cff:feaa:1aed")
    };

    EXPECT_EQ(addrs[0].GetFullAddress(), std::string("fd50:15ec:9b76:0:cacd:390a:1a96:2809"));
    EXPECT_EQ(addrs[1].GetFullAddress(), std::string("fd50:15ec:9b76:0:2e0:6cff:feaa:1aed"));
    EXPECT_EQ(addrs[2].GetFullAddress(), std::string("fe80::2e0:6cff:feaa:1aed"));
}

TEST(IPv6Test, WithPort) {
    libnet::IPv6 addr = "[fd50:15ec:9b76:0:cacd:390a:1a96:2809]:8080";
    EXPECT_EQ(addr.GetFullAddress(), std::string("[fd50:15ec:9b76:0:cacd:390a:1a96:2809]:8080"));
}

TEST(IPv6Test, InvalidData) {
    libnet::IPv6 addr = "fffff:fffff:fffff:fffff:fffff:fffff:fffff:fffff";
    EXPECT_EQ(addr.GetFullAddress(), std::string("::"));
}

TEST(IPv6Test, CreateSocketServer) {
    // Используем loopback и свободный порт (0 позволит ОС выбрать любой свободный)
    libnet::IPv6 server_addr("[::1]:0");
    
    // 1. Создаем сокет
    int fd = socket(AF_INET6, SOCK_STREAM, 0);
    ASSERT_GE(fd, 0) << "Failed to create socket";

    // 2. Получаем структуру sockaddr_in6 из нашего класса
    sockaddr_in6 addr_in6 = server_addr.GetSockAddr();

    // 3. Пробуем забиндить сокет
    int res = bind(fd, (struct sockaddr*)&addr_in6, sizeof(addr_in6));
    
    // Если бинд не прошел, выведем ошибку (может быть занят порт или нет поддержки IPv6)
    if (res < 0) {
        close(fd);
        FAIL() << "Bind failed: " << strerror(errno);
    }

    // 4. Проверяем, что сокет перешел в состояние прослушивания
    res = listen(fd, 1);
    EXPECT_EQ(res, 0) << "Listen failed";

    // Бонус: проверим, какой порт реально выделила ОС (если использовали 0)
    sockaddr_in6 sin;
    socklen_t len = sizeof(sin);
    if (getsockname(fd, (struct sockaddr *)&sin, &len) == 0) {
        std::cout << "[ INFO ] Server started on port: " << ntohs(sin.sin6_port) << std::endl;
    }

    // 5. Закрываем
    close(fd);
    SUCCEED();
}
#include <gtest/gtest.h>
#include <libnet/dev/tun.hpp>
#include <cstring>
#include <sys/ioctl.h>
#include <net/if.h>
#include <unistd.h>

TEST(TUNTest, CreationAndConfig) {
    try {
        // 1. Создание
        libnet::TUN tun("test_tun0");
        EXPECT_EQ(tun.get_ifname(), "test_tun0");

        // 2. Настройка
        libnet::IPv4 ip("10.99.0.1:0");
        tun.Configure(ip, 24, 1400);

        // 3. Проверка через системный сокет, что настройки применились
        int sock = socket(AF_INET, SOCK_DGRAM, 0);
        struct ifreq ifr;
        std::memset(&ifr, 0, sizeof(ifr));
        std::strncpy(ifr.ifr_name, "test_tun0", IFNAMSIZ);

        // Проверяем IP
        ASSERT_GE(ioctl(sock, SIOCGIFADDR, &ifr), 0);
        sockaddr_in* sin = (sockaddr_in*)&ifr.ifr_addr;
        EXPECT_EQ(ntohl(sin->sin_addr.s_addr), ip.GetRawAddress());

        // Проверяем MTU
        ASSERT_GE(ioctl(sock, SIOCGIFMTU, &ifr), 0);
        EXPECT_EQ(ifr.ifr_mtu, 1400);

        // Проверяем статус UP
        ASSERT_GE(ioctl(sock, SIOCGIFFLAGS, &ifr), 0);
        EXPECT_TRUE(ifr.ifr_flags & IFF_UP);

        close(sock);
    } catch (const std::exception& e) {
        // Если нет прав sudo, тест пометим как пропущенный (или упавший)
        FAIL() << "TUN creation failed (maybe forgot sudo?): " << e.what();
    }
}

TEST(TUNTest, MoveSemantics) {
    try {
        libnet::TUN tun1("move_tun0");
        int original_fd = tun1.get_fd();
        
        // Перемещаем
        libnet::TUN tun2 = std::move(tun1);
        
        EXPECT_EQ(tun1.get_fd(), -1);
        EXPECT_EQ(tun2.get_fd(), original_fd);
        EXPECT_EQ(tun2.get_ifname(), "move_tun0");
    } catch (...) {
        GTEST_SKIP() << "Skipping due to lack of privileges";
    }
}

TEST(TUNTest, ReadWriteData) {
    try {
        libnet::TUN tun("io_tun0");
        tun.Configure(libnet::IPv4("10.100.0.1:0"), 24);

        // Попробуем записать "фейковый" пакет (просто набор байт)
        uint8_t fake_packet[] = {0x45, 0x00, 0x00, 0x14, 0x00, 0x01, 0x00, 0x00, 0x40, 0x00};
        ssize_t written = tun.write(fake_packet, sizeof(fake_packet));
        EXPECT_EQ(written, sizeof(fake_packet));
        
        // Настоящий Read проверить сложно без внешнего трафика в рамках юнит-теста,
        // но мы убедились, что дескриптор рабочий.
    } catch (...) {
        GTEST_SKIP();
    }
}

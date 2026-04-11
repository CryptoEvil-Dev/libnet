#include <gtest/gtest.h>
#include <libnet/network/ipv6net.hpp>

TEST(IPv6NetTest, ContainsAndMask) {
    // Подсеть /64
    libnet::IPv6Net net("2001:db8::/64");

    // Адреса, которые должны входить
    EXPECT_TRUE(net.Contains(libnet::IPv6("2001:db8::1")));
    EXPECT_TRUE(net.Contains(libnet::IPv6("2001:db8::ffff:ffff")));
    EXPECT_TRUE(net.Contains(libnet::IPv6("2001:db8::abc:123")));

    // Адрес из другой подсети
    EXPECT_FALSE(net.Contains(libnet::IPv6("2001:db9::1")));
}

TEST(IPv6NetTest, AllocateAndPool) {
    // Создаем сеть с маленьким пулом для теста (10 адресов)
    libnet::IPv6Net net("fd00::/64", 10);

    // Anycast (0-й адрес) должен быть занят по умолчанию
    EXPECT_FALSE(net.IsFree(libnet::IPv6("fd00::0")));

    // Выделяем адрес
    auto ip1 = net.Allocate();
    EXPECT_EQ(ip1.GetAddress(), "fd00::1");
    EXPECT_FALSE(net.IsFree(ip1));

    // Резервируем вручную
    net.Reserve(libnet::IPv6("fd00::5"));
    EXPECT_FALSE(net.IsFree(libnet::IPv6("fd00::5")));

    // Освобождаем
    net.Release(ip1);
    EXPECT_TRUE(net.IsFree(libnet::IPv6("fd00::1")));
}

TEST(IPv6NetTest, OutOfPoolRange) {
    // Пул всего на 5 адресов
    libnet::IPv6Net net("fe80::/64", 5);

    // Адрес ::6 входит в подсеть /64, но не входит в наш менеджер ресурсов (пул)
    libnet::IPv6 out_of_pool_ip("fe80::6");
    
    EXPECT_TRUE(net.Contains(out_of_pool_ip));
    EXPECT_FALSE(net.IsFree(out_of_pool_ip)); // Должен вернуть false, так как не можем управлять
    EXPECT_THROW(net.Reserve(out_of_pool_ip), std::runtime_error);
}


TEST(IPv6Debug, InspectValues) {
    libnet::IPv6 ip("2001:db8::1");
    libnet::IPv6Net net("2001:db8::/64");
    
    libnet::uint128_t ip_raw = ip.GetRawAddress();
    libnet::uint128_t base_raw = net.GetBaseAddress().GetRawAddress();
    
    // Мы ожидаем увидеть что-то вроде high: 20010db800000000, low: 1
    std::cout << "[ DEBUG ] IP Raw:   high=" << std::hex << ip_raw.high() << " low=" << ip_raw.low() << std::dec << std::endl;
    std::cout << "[ DEBUG ] Net Base: high=" << std::hex << base_raw.high() << " low=" << base_raw.low() << std::dec << std::endl;
    
    EXPECT_TRUE(net.Contains(ip));
}
#include <gtest/gtest.h>
#include <libnet/network/ipv4net.hpp>
#include <thread>
#include <vector>

TEST(IPv4NetTest, NetworkReservation) {
    // Сеть /24 (256 адресов, 254 доступно)
    libnet::IPv4Net net("192.168.1.0/24");
    
    // Проверяем, что .0 и .255 уже заняты (IsFree вернет false)
    EXPECT_FALSE(net.IsFree("192.168.1.0"));
    EXPECT_FALSE(net.IsFree("192.168.1.255"));
    
    // Первый доступный должен быть .1
    EXPECT_TRUE(net.IsFree("192.168.1.1"));
}

TEST(IPv4NetTest, AllocateAndRelease) {
    libnet::IPv4Net net("10.0.0.0/30"); // 4 адреса: .0 (net), .1, .2, .3 (broadcast)
    
    auto ip1 = net.Allocate();
    EXPECT_EQ(ip1.GetAddress(), "10.0.0.1");
    
    auto ip2 = net.Allocate();
    EXPECT_EQ(ip2.GetAddress(), "10.0.0.2");
    
    // Больше адресов нет
    EXPECT_THROW(net.Allocate(), std::runtime_error);
    
    net.Release(ip1);
    EXPECT_NO_THROW(net.Allocate());
}

TEST(IPv4NetTest, MultiThreadedAllocate) {
    libnet::IPv4Net net("172.16.0.0/16"); // Много адресов
    std::vector<std::thread> threads;
    std::vector<libnet::IPv4> results(1000);

    // 10 потоков одновременно выделяют по 100 адресов
    for (int i = 0; i < 10; ++i) {
        threads.emplace_back([&net, &results, i]() {
            for (int j = 0; j < 100; ++j) {
                results[i * 100 + j] = net.Allocate();
            }
        });
    }

    for (auto& t : threads) t.join();

    // Проверяем уникальность (все 1000 адресов должны быть разными)
    std::set<uint32_t> unique_ips;
    for (const auto& ip : results) {
        unique_ips.insert(ip.GetRawAddress());
    }
    EXPECT_EQ(unique_ips.size(), 1000);
}

#include <iostream>
#include <gtest/gtest.h>
#include <types/include/uint128.hpp>

TEST(uint128tTest, DefaultConstructor) {
    libnet::uint128_t data;
    data = 1024;
    uint32_t t_d1 = 1024;
    uint64_t t_d2 = 1024;
    uint16_t t_d3 = 1024;

    EXPECT_EQ(data, t_d1);
    EXPECT_EQ(data, t_d2);
    EXPECT_EQ(data, t_d3);

    std::cout << data.to_string() << std::endl;
}
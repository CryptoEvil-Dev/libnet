#include <gtest/gtest.h>
#include <libnet/core/bitset.hpp>

TEST(BitsetTest, BasicOps) {
    libnet::bitset bs(100);
    
    EXPECT_FALSE(bs.test(10));
    bs.set(10);
    EXPECT_TRUE(bs.test(10));
    
    bs.reset(10);
    EXPECT_FALSE(bs.test(10));
}

TEST(BitsetTest, BoundaryConditions) {
    libnet::bitset bs(64); // Ровно одно слово uint64_t
    bs.set(0);
    bs.set(63);
    
    EXPECT_TRUE(bs.test(0));
    EXPECT_TRUE(bs.test(63));
    EXPECT_TRUE(bs.test(64)); // Out of bounds должен возвращать true (занято)
}

TEST(BitsetTest, FindFirstFree) {
    libnet::bitset bs(10);
    // Сначала все свободны, должен вернуть 0
    auto f1 = bs.FindFirstFree();
    ASSERT_TRUE(f1.has_value());
    EXPECT_EQ(*f1, 0);

    // Забиваем первые 3 бита
    bs.set(0);
    bs.set(1);
    bs.set(2);
    
    auto f2 = bs.FindFirstFree();
    EXPECT_EQ(*f2, 3);
}

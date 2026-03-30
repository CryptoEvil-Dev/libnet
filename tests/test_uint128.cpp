#include <iostream>
#include <gtest/gtest.h>
#include <types/include/uint128.hpp>

// 1. Тест конструкторов и базового вывода
TEST(UInt128Test, ConstructionAndString) {
    // Из одного числа
    libnet::uint128_t a(1024);
    EXPECT_EQ(a.to_string(), "1024");
    EXPECT_EQ(a.low(), 1024);
    EXPECT_EQ(a.high(), 0);

    // Из двух половин (High, Low)
    // 1 << 64 это 18446744073709551616
    libnet::uint128_t b(1, 0); 
    EXPECT_EQ(b.to_string(), "18446744073709551616");
    EXPECT_EQ(b.high(), 1);
    EXPECT_EQ(b.low(), 0);
}

// 2. Тест парсинга строк (std::string_view)
TEST(UInt128Test, StringParsing) {
    libnet::uint128_t val("340282366920938463463374607431768211455"); // Max uint128
    EXPECT_EQ(val.to_string(), "340282366920938463463374607431768211455");
    
    // Проверка на 0
    libnet::uint128_t zero("0");
    EXPECT_EQ(zero, 0);
}

// 3. Арифметика и переполнение
TEST(UInt128Test, Arithmetic) {
    libnet::uint128_t a(0, 0xFFFFFFFFFFFFFFFF); // Max uint64
    libnet::uint128_t b(1);
    
    libnet::uint128_t res = a + b;
    // Результат должен стать 1 в старшей части и 0 в младшей
    EXPECT_EQ(res.high(), 1);
    EXPECT_EQ(res.low(), 0);
    
    libnet::uint128_t diff = res - b;
    EXPECT_EQ(diff, a);
}

// 4. Побитовые операции и сдвиги
TEST(UInt128Test, BitwiseOperations) {
    libnet::uint128_t val(0, 0b1010);
    
    // Сдвиг влево на 64 (переход в high)
    auto shifted = val << 64;
    EXPECT_EQ(shifted.high(), 0b1010);
    EXPECT_EQ(shifted.low(), 0);

    // Побитовое И
    libnet::uint128_t mask(0, 0b1100);
    EXPECT_EQ((val & mask), 0b1000);
    
    // Инверсия
    libnet::uint128_t zero(0);
    libnet::uint128_t full = ~zero;
    EXPECT_EQ(full.high(), 0xFFFFFFFFFFFFFFFF);
    EXPECT_EQ(full.low(), 0xFFFFFFFFFFFFFFFF);
}

// 5. Сравнения
TEST(UInt128Test, Comparisons) {
    libnet::uint128_t small(100);
    libnet::uint128_t big(1, 0);

    EXPECT_TRUE(big > small);
    EXPECT_TRUE(small < big);
    EXPECT_TRUE(small == 100);
    EXPECT_FALSE(small != 100);
}

// 6. Хеширование и использование в контейнерах
TEST(UInt128Test, HashAndContainers) {
    std::unordered_map<libnet::uint128_t, std::string> nodes;
    
    libnet::uint128_t key1("12345678901234567890");
    libnet::uint128_t key2("98765432109876543210");
    
    nodes[key1] = "first";
    nodes[key2] = "second";
    
    EXPECT_EQ(nodes.size(), 2);
    EXPECT_EQ(nodes[key1], "first");
}

// 7. Потоковый вывод
TEST(UInt128Test, StreamOutput) {
    libnet::uint128_t val(12345);
    std::stringstream ss;
    ss << val;
    EXPECT_EQ(ss.str(), "12345");
}
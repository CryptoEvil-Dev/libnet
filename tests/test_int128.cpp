#include <iostream>
#include <gtest/gtest.h>
#include <libnet/core/int128.hpp>

// 1. Тест конструкторов и базового вывода
TEST(UInt128Test, ConstructionAndString) {
    // Из одного числа
    libnet::int128_t a(1024);
    EXPECT_EQ(a.to_string(), "1024");
    EXPECT_EQ(a.low(), 1024);
    EXPECT_EQ(a.high(), 0);

    // Из двух половин (High, Low)
    // 1 << 64 это 18446744073709551616
    libnet::int128_t b(1, 0); 
    EXPECT_EQ(b.to_string(), "18446744073709551616");
    EXPECT_EQ(b.high(), 1);
    EXPECT_EQ(b.low(), 0);
}

// 2. Тест парсинга строк (std::string_view)
TEST(Int128Test, StringParsing) {
    // Максимум для знакового int128 (2^127 - 1)
    std::string max_val = "170141183460469231731687303715884105727";
    libnet::int128_t val(max_val);
    EXPECT_EQ(val.to_string(), max_val);
    
    // Проверка отрицательного числа
    libnet::int128_t neg("-170141183460469231731687303715884105728");
    EXPECT_EQ(neg.to_string(), "-170141183460469231731687303715884105728");

    libnet::int128_t zero("0");
    EXPECT_EQ(zero, 0);
}

// 3. Арифметика и переполнение
TEST(UInt128Test, Arithmetic) {
    libnet::int128_t a(0, 0xFFFFFFFFFFFFFFFF); // Max uint64
    libnet::int128_t b(1);
    
    libnet::int128_t res = a + b;
    // Результат должен стать 1 в старшей части и 0 в младшей
    EXPECT_EQ(res.high(), 1);
    EXPECT_EQ(res.low(), 0);
    
    libnet::int128_t diff = res - b;
    EXPECT_EQ(diff, a);
}

// 4. Побитовые операции и сдвиги
TEST(UInt128Test, BitwiseOperations) {
    libnet::int128_t val(0, 0b1010);
    
    // Сдвиг влево на 64 (переход в high)
    auto shifted = val << 64;
    EXPECT_EQ(shifted.high(), 0b1010);
    EXPECT_EQ(shifted.low(), 0);

    // Побитовое И
    libnet::int128_t mask(0, 0b1100);
    EXPECT_EQ((val & mask), 0b1000);
    
    // Инверсия
    libnet::int128_t zero(0);
    libnet::int128_t full = ~zero;
    EXPECT_EQ(full.high(), 0xFFFFFFFFFFFFFFFF);
    EXPECT_EQ(full.low(), 0xFFFFFFFFFFFFFFFF);
}

// 5. Сравнения
TEST(UInt128Test, Comparisons) {
    libnet::int128_t small(100);
    libnet::int128_t big(1, 0);

    EXPECT_TRUE(big > small);
    EXPECT_TRUE(small < big);
    EXPECT_TRUE(small == 100);
    EXPECT_FALSE(small != 100);
}

// 6. Хеширование и использование в контейнерах
TEST(UInt128Test, HashAndContainers) {
    std::unordered_map<libnet::int128_t, std::string> nodes;
    
    libnet::int128_t key1("12345678901234567890");
    libnet::int128_t key2("98765432109876543210");
    
    nodes[key1] = "first";
    nodes[key2] = "second";
    
    EXPECT_EQ(nodes.size(), 2);
    EXPECT_EQ(nodes[key1], "first");
}

// 7. Потоковый вывод
TEST(UInt128Test, StreamOutput) {
    libnet::int128_t val(12345);
    std::stringstream ss;
    ss << val;
    EXPECT_EQ(ss.str(), "12345");
}

TEST(UInt128Test, DifferentConstructors) {
    libnet::int128_t data = 1024;
    
    libnet::int128_t udata_128 = __int128_t(1024);
    libnet::int128_t udata_64 = uint64_t(1024);
    libnet::int128_t udata_32 = uint32_t(1024);
    libnet::int128_t udata_16 = uint16_t(1024);

    libnet::int128_t sdata_128 = __int128_t(1024);
    libnet::int128_t sdata_64 = int64_t(1024);
    libnet::int128_t sdata_32 = int32_t(1024);
    libnet::int128_t sdata_16 = int16_t(1024);

    EXPECT_EQ(data, udata_128);
    EXPECT_EQ(data, udata_64);
    EXPECT_EQ(data, udata_32);
    EXPECT_EQ(data, udata_16);

    EXPECT_EQ(data, sdata_128);
    EXPECT_EQ(data, sdata_64);
    EXPECT_EQ(data, sdata_32);
    EXPECT_EQ(data, sdata_16);
}

TEST(UInt128Test, HeapTest) {
    libnet::int128_t* data = new libnet::int128_t(1024);
    EXPECT_EQ(*data, 1024);

    delete data;
    data = nullptr;

    libnet::int128_t* _dt = new libnet::int128_t[32];
    for(size_t i = 0; i < 32; ++i) {
        _dt[i] = i;
    }

    for(size_t i = 0; i < 32; ++i) {
        EXPECT_EQ(_dt[i], i);
    }

    delete[] _dt;
    _dt = nullptr;
}
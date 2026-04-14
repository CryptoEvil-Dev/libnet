#include <gtest/gtest.h>
#include <libnet/network/packet.hpp>

TEST(IPPacketTest, ManualBuild) {
    libnet::IPv4 src("10.8.0.2:0");
    libnet::IPv4 dest("8.8.8.8:0");
    std::string payload = "Hello, Packet!";
    
    // Собираем пакет
    libnet::IPPacket pkt(libnet::UDP, src, dest, payload);
    
    EXPECT_TRUE(pkt.IsValid());
    EXPECT_EQ(pkt.GetProtocol(), libnet::UDP);
    EXPECT_EQ(pkt.GetSource().GetAddress(), "10.8.0.2");
    EXPECT_EQ(pkt.GetDest().GetAddress(), "8.8.8.8");
    EXPECT_EQ(pkt.GetPayloadSize(), payload.size());
    
    // Проверка, что данные на месте
    std::string received_payload((const char*)pkt.GetPayload(), pkt.GetPayloadSize());
    EXPECT_EQ(received_payload, payload);
}

TEST(IPPacketTest, ParsingFromRaw) {
    // Реальный дамп IPv4 пакета (20 байт заголовок + 4 байта данные)
    // 45 00 00 18 ...
    std::vector<uint8_t> raw = {
        0x45, 0x00, 0x00, 0x18, 0x00, 0x00, 0x40, 0x00, 
        0x40, 0x11, 0xb6, 0x2d, 0x0a, 0x08, 0x00, 0x01, // src: 10.8.0.1
        0x08, 0x08, 0x08, 0x08, 0x41, 0x42, 0x43, 0x44  // dest: 8.8.8.8, data: ABCD
    };

    libnet::IPPacket pkt(raw);
    
    EXPECT_TRUE(pkt.IsValid());
    EXPECT_EQ(pkt.GetSource().GetAddress(), "10.8.0.1");
    EXPECT_EQ(pkt.GetDest().GetAddress(), "8.8.8.8");
    EXPECT_EQ(pkt.GetPayloadSize(), 4);
}

#pragma once
#include <libnet/network/ipv4.hpp>
#include <vector>
#include <cstdint>

namespace libnet {

enum Protocol : uint8_t {
    ICMP = 1,
    TCP = 6,
    UDP = 17
};

class IPPacket {
private:
    std::vector<uint8_t> _raw;
    uint16_t CalculateChecksum(const uint8_t* data, size_t len);

public:
    IPPacket() = default;
    IPPacket(std::vector<uint8_t> data) : _raw(std::move(data)) {};

    IPPacket(Protocol proto, const IPv4& src, const IPv4& dest, std::string_view payload);

    IPPacket(const IPPacket&) = default;
    IPPacket(IPPacket&&) = default;

    IPPacket& operator = (const IPPacket&) = default;
    IPPacket& operator = (IPPacket&&) = default;
    ~IPPacket() = default;

    // Геттеры заголовка IPv4
    uint8_t GetVersion() const;
    uint8_t GetHeaderLength() const;
    uint8_t GetProtocol() const;
    uint16_t GetTotalLength() const;

    libnet::IPv4 GetSource() const;

    libnet::IPv4 GetDest() const;

    // Полезная нагрузка (Данные после заголовка IP)
    const uint8_t* GetPayload() const;
    size_t GetPayloadSize() const;

    // Экспорт в разные форматы
    const std::vector<uint8_t>& ToVector() const;
    std::string ToString() const;

    bool IsValid() const;
    size_t Size() const;
};

}
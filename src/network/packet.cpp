#include <libnet/network/packet.hpp>
#include <cstring>

libnet::IPPacket::IPPacket(Protocol proto, const IPv4& src, const IPv4& dest, std::string_view payload) {
    _raw.resize(20 + payload.size());

    _raw[0] = 0x45; // Version 4, IHL 5 (20 bytes)
    _raw[1] = 0x00; // DSCP/ECN

    uint16_t total_len = static_cast<uint16_t>(_raw.size());
    _raw[2] = total_len >> 8;
    _raw[3] = total_len & 0xFF;

    // Identification, Flags, Fragment Offset (по нулям)
    std::memset(&_raw[4], 0, 4);

    _raw[8] = 64; // TTL
    _raw[9] = proto; // Protocol

    // Source IP
    uint32_t s_addr = htonl(src.GetRawAddress());
    std::memcpy(&_raw[12], &s_addr, 4);

    // Dest IP
    uint32_t d_addr = htonl(dest.GetRawAddress());
    std::memcpy(&_raw[16], &d_addr, 4);

    // Checksum (Изначально 0)
    _raw[10] = 0; _raw[11] = 0;
    uint16_t checksum = CalculateChecksum(_raw.data(), 20);
    _raw[10] = checksum >> 8;
    _raw[11] = checksum & 0xFF;

    // Payload
    std::memcpy(&_raw[20], payload.data(), payload.size());
}

uint16_t libnet::IPPacket::CalculateChecksum(const uint8_t* data, size_t len) {
    uint32_t sum = 0;
    const uint16_t* ptr = reinterpret_cast<const uint16_t*>(data);

    for(size_t i = 0; i < len / 2; ++i){
        sum += ntohs(ptr[i]);
    }

    while (sum >> 16)
    {
        sum = (sum & 0xFFFF) + (sum >> 16);
    }

    return htons(static_cast<uint16_t>(~sum));
    
}

uint8_t libnet::IPPacket::GetVersion() const {
    return (_raw[0] >> 4);
}

uint8_t libnet::IPPacket::GetHeaderLength() const {
    return (_raw[0] & 0x0F) * 4;
}

uint8_t libnet::IPPacket::GetProtocol() const {
    return _raw[9];
}

uint16_t libnet::IPPacket::GetTotalLength() const {
    return (_raw[2] << 8) | _raw[3];
}

libnet::IPv4 libnet::IPPacket::GetSource() const {
    uint32_t addr;
    std::memcpy(&addr, &_raw[12], 4);
    return IPv4(ntohl(addr), 0);
}

libnet::IPv4 libnet::IPPacket::GetDest() const {
    uint32_t addr;
    std::memcpy(&addr, &_raw[16], 4);
    return IPv4(ntohl(addr), 0);
}

const uint8_t* libnet::IPPacket::GetPayload() const {
    return _raw.data() + GetHeaderLength();
}

size_t libnet::IPPacket::GetPayloadSize() const {
    return _raw.size() - GetHeaderLength();
}

const std::vector<uint8_t>& libnet::IPPacket::ToVector() const {
    return _raw;
}

std::string libnet::IPPacket::ToString() const {
    return std::string(_raw.begin(), _raw.end());
}

bool libnet::IPPacket::IsValid() const {
    return _raw.size() >= 20 && GetVersion() == 4;
}

size_t libnet::IPPacket::Size() const {
    return _raw.size();
}
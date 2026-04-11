#pragma once

#include <libnet/network/ipv4.hpp>
#include <string>
#include <vector>

namespace libnet {

class TUN {
private:
    int _fd;
    std::string _ifname;

public:
    TUN(const std::string& ifname = "libnet_tun0");
    ~TUN();

    TUN(const TUN&) = delete;
    TUN& operator = (const TUN&) = delete;

    TUN(TUN&&) noexcept;
    TUN& operator = (TUN&&) noexcept;

    void Configure(const libnet::IPv4& addr, uint8_t prefix, int mtu = 1420);

    ssize_t write(const void* data, size_t len);
    std::vector<uint8_t> read(size_t max_len = 4096);

    int get_fd() const noexcept { return _fd; }
    std::string get_ifname() const noexcept { return _ifname; }

};

}
#include <libnet/network/ipv4.hpp>

libnet::IPv4::IPv4(const char* address) {
    _addr = 0;
    _port = 0;
    uint8_t buffer = 0;
    uint8_t shift = 24;
    
    for (uint8_t i = 0; address[i] != '\0'; ++i){
        if(address[i] == '.') {
            _addr |= buffer << shift;
            buffer = 0;
            shift -= 8;
        } else if(address[i] == ':') {
            _addr |= buffer;
            for(uint8_t _ip = i + 1; address[_ip] != '\0'; ++_ip) {
                _port = _port * 10 + (address[_ip] - '0');
            }
            return;
        } else {
            buffer = buffer * 10 + (address[i] - '0');
        }
    }
    _addr |= buffer;
}

std::string libnet::IPv4::GetAddress() const noexcept {
    uint8_t addr[4] = {0};
    addr[0] = (_addr >> 24) & 0xFF;
    addr[1] = (_addr >> 16) & 0xFF;
    addr[2] = (_addr >>  8) & 0xFF;
    addr[3] = _addr & 0xFF;
    
    return
        std::to_string(addr[0]) + '.' +
        std::to_string(addr[1]) + '.' +
        std::to_string(addr[2]) + '.' +
        std::to_string(addr[3]);
}

std::string libnet::IPv4::GetFullAddress() const noexcept {
    if(_port > 0){
        return this->GetAddress() + ':' + std::to_string(_port);
    }
    return this->GetAddress();
}

sockaddr_in libnet::IPv4::GetSockAddr() const noexcept {
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(_port);
    addr.sin_addr.s_addr = htonl(_addr);
    return addr;
}
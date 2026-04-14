#include <libnet/libnet.hpp>
#include <iostream>

int main() {
    try {
        libnet::TUN tun("libnet_demo");
        tun.Configure(libnet::IPv4("10.10.10.1"), 24);
        
        std::cout << "Интерфейс " << tun.get_ifname() << " поднят на 10.10.10.1" << std::endl;
        std::cout << "Выполни в другом терминале: ping 10.10.10.1" << std::endl;

        while (true) {
            auto raw = tun.read();
            libnet::IPPacket pkt(raw);
            
            if (pkt.IsValid()) {
                std::cout << "[IP] " << pkt.GetSource().GetAddress() 
                          << " -> " << pkt.GetDest().GetAddress() 
                          << " (Protocol: " << (int)pkt.GetProtocol() << ")" << std::endl;
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << " (Забыли sudo?)" << std::endl;
    }
    return 0;
}

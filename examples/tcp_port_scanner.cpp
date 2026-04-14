#include <libnet/libnet.hpp>
#include <iostream>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Usage: ./tcp_port_scanner <IP>" << std::endl;
        return 1;
    }

    std::string target_ip = argv[1];
    std::vector<uint16_t> common_ports = {21, 22, 80, 443, 8080, 3306};

    std::cout << "Сканируем " << target_ip << "..." << std::endl;

    for (uint16_t port : common_ports) {
        try {
            libnet::TCPConnection conn;
            // Пытаемся подключиться (таймаут тут пока системный)
            conn.connect(libnet::IPv4(target_ip, port));
            std::cout << "[+] Port " << port << " is OPEN" << std::endl;
        } catch (...) {
            std::cout << "[-] Port " << port << " is closed" << std::endl;
        }
    }
    return 0;
}

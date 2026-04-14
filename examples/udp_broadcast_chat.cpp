#include <libnet/libnet.hpp>
#include <iostream>
#include <thread>

int main() {
    // Слушаем на порту 9000
    libnet::UDPSocket sock(libnet::IPv4("0.0.0.0:9000"));
    std::cout << "Чат запущен на порту 9000. Пиши сообщения!" << std::endl;

    // Поток для чтения входящих сообщений
    std::thread receiver([&]() {
        while (true) {
            auto [from, data] = sock.recv();
            std::string msg(data.begin(), data.end());
            std::cout << "\n[" << from.GetAddress() << "]: " << msg << std::endl;
        }
    });

    // Основной цикл для отправки (на широковещательный адрес сети)
    libnet::IPv4 broadcast_addr("255.255.255.255:9000");
    std::string line;
    while (std::getline(std::cin, line)) {
        sock.sendto(broadcast_addr, line.c_str(), line.size());
    }

    receiver.join();
    return 0;
}

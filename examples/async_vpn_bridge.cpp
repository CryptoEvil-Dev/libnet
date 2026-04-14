#include <libnet/libnet.hpp>
#include <iostream>

int main() {
    try {
        libnet::TUN tun("libnet_vpn");
        tun.Configure(libnet::IPv4("10.8.0.2"), 24);

        libnet::UDPSocket udp_sock(libnet::IPv4("0.0.0.0:0")); // На любой свободный порт
        libnet::IPv4 vpn_server("1.2.3.4:1194"); // Адрес твоего сервера

        libnet::EventLoop loop;

        // 1. Из туннеля (от ОС) -> Шифруем и в сокет
        loop.Add(tun.get_fd(), [&](int fd) {
            auto packet = tun.read();
            std::cout << "-> Отправляем пакет на сервер (" << packet.size() << " байт)" << std::endl;
            udp_sock.sendto(vpn_server, packet.data(), packet.size());
        });

        // 2. Из сокета (от сервера) -> Расшифровываем и в туннель (в ОС)
        loop.Add(udp_sock.get_fd(), [&](int fd) {
            auto [from, data] = udp_sock.recv();
            std::cout << "<- Получен ответ от сервера" << std::endl;
            tun.write(data.data(), data.size());
        });

        std::cout << "VPN Мост запущен. Весь трафик 10.8.0.x идет через UDP." << std::endl;
        loop.Run();

    } catch (const std::exception& e) {
        std::cerr << "Критическая ошибка: " << e.what() << std::endl;
    }
    return 0;
}

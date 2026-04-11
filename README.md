# libnet

Легковесная и быстрая C++ библиотека для работы с сетевыми абстракциями, туннелями и адресацией в Linux. Спроектирована для тех, кто хочет создавать VPN, прокси-серверы и кастомные сетевые стеки без лишней боли.

## 🚀 Основные возможности

- **Умная адресация**: Полноценные объекты `IPv4` и `IPv6` с поддержкой парсинга из строк (CIDR) и интеграцией с системными `sockaddr`.
- **Менеджмент сетей**: `IPv4Net` и `IPv6Net` на базе эффективного `bitset`. Позволяет выделять (`Allocate`), резервировать (`Reserve`) и проверять адреса в подсетях.
- **Масштабируемые типы**: Собственная реализация `uint128_t` и `int128_t` для IPv6.
- **Виртуальные интерфейсы**: Простая работа с Linux **TUN** устройствами. Создание, настройка IP/MTU и чтение сырых пакетов в несколько строк кода.
- **Универсальные сокеты**: `UDPSocket` с поддержкой Dual-stack (v4/v6) и удобным интерфейсом отправки/приема данных.

## 📁 Структура проекта

* `include/libnet/core` — базовые типы данных и битовые карты.
* `include/libnet/network` — логика работы с IP и подсетями.
* `include/libnet/dev` — сетевые устройства и сокеты.

## 🛠 Сборка

Библиотека использует CMake и может быть собрана как локально, так и в изолированном Docker-контейнере.

### Локально:
```bash
cmake --workflow --preset=build_all
```

### Через Docker (если лень ставить зависимости):
```bash
docker compose up -d --build
```

## 💻 Примеры использования

### Создание TUN-туннеля и чтение пакетов

```cpp
#include <libnet/libnet.hpp>

int main() {
    // Создаем туннель и настраиваем IP
    libnet::TUN tun("vpn_tun0");
    tun.Configure(libnet::IPv4("10.8.0.1"), 24);

    while (true) {
        auto packet = tun.read(); // Получаем сырой IP-пакет
        std::cout << "Received packet, size: " << packet.size() << " bytes" << std::endl;
    }
}
```

### Работа с UDP-сокетом

```cpp
libnet::UDPSocket sock(libnet::IPv4("0.0.0.0:8080"));

while (true) {
    auto msg = sock.recv();
    std::cout << "Message from " << std::get<libnet::IPv4>(msg.addr).GetAddress() << std::endl;
    
    // Эхо-ответ
    sock.sendto(msg);
}
```

## 🧪 Тестирование

Проект покрыт юнит-тестами (Google Test).
*Примечание: тесты для TUN требуют прав sudo или CAP_NET_ADMIN.*

```bash
sudo ./build/build_all/tests/test_tun
./build/build_all/tests/test_ipv4net
```

## 📋 Roadmap

- Реализация IPv6Net
- TCPClient & TCPServer абстракции
- Поддержка TAP устройств (L2)
- Асинхронный Event Loop (epoll)

---
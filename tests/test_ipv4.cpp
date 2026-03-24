#include <iostream>
#include <cassert>
#include <sys/socket.h>
#include <unistd.h>

#include <gtest/gtest.h>
#include <types/include/ipv4.hpp>

TEST(IPv4Test, DefaultConstructor) {
    libnet::IPv4 addr;
    // std::cout << "Size: " << sizeof(addr) << " bytes" << std::endl;
    EXPECT_EQ(sizeof(addr), 8);
    EXPECT_EQ(addr.GetFullAddress(), std::string("0.0.0.0"));
}

TEST(IPv4Test, Overflow) {
    libnet::IPv4 addr("256.256.256.256:65536");
    EXPECT_EQ(addr.GetFullAddress(), std::string("0.0.0.0"));
}

TEST(IPv4Test, CreateSocketServer) {
    libnet::IPv4 addr("127.0.0.1:3500");
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    
    if(sock < 0) {
        perror("Socker creation failed");
        FAIL();
    }

    sockaddr_in s_addr = addr.GetSockAddr();
    if (bind(sock, (struct sockaddr*)&s_addr, sizeof(s_addr)) < 0) {
        perror("Bind failed");
        close(sock);
        FAIL();
    }

    close(sock);
    SUCCEED();
}
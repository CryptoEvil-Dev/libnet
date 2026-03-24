#include "models/include/ippool.hpp"


libnet::PeerInfo libnet::IPPool::acquire(const libnet::IPv4 peer) {
    auto it = peers.find(peer);
    if(it != peers.end()) {
        return it->second;
    }

    libnet::IPv4 virt(next_v_addr++, 0);
}



libnet::PeerInfo libnet::IPPool::lookup(const libnet::IPv4 peer) noexcept {
    auto it = peers.find(peer);
    if(it != this->peers.end()) {
        return it->second;
    }
    return {};
}
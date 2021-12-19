
#include <arpa/inet.h>
#include <cstring>
#include <memory>
#include "address.hpp"

Address::Address(const std::string &host_address) {
    struct addrinfo hints{.ai_family = AF_UNSPEC, .ai_socktype = SOCK_STREAM};

    //todo error handling
    if (0 != getaddrinfo(host_address.c_str(), "http", &hints, &address_info)) {
        throw std::runtime_error("Could not get adressinfo of '" + host_address + "'");
    }

    sockaddr_in_use = address_info->ai_addr;
    sockaddr_in_use_length = address_info->ai_addrlen;
}

Address::Address(struct sockaddr *sockaddr, socklen_t socklen) : address_info(nullptr),
                                                                 sockaddr_in_use(sockaddr),
                                                                 sockaddr_in_use_length(socklen) {

}

Address::~Address() {
    if (nullptr != address_info) {
        freeaddrinfo(address_info);
        sockaddr_in_use = nullptr;
    } else {
        delete sockaddr_in_use;
    }
}

const sockaddr *Address::get_sockaddr() const {
    return sockaddr_in_use;
}

socklen_t Address::get_socklen() const {
    return sockaddr_in_use_length;
}

std::string Address::str() const {
    return addr_to_string(get_sockaddr());
}

void Address::print_all_ip_addresses() const {
    if (address_info == nullptr) {
        std::cout << addr_to_string(get_sockaddr());
        return;
    }

    for (addrinfo *p = address_info; p != nullptr; p = p->ai_next) {
        std::cout << addr_to_string(p->ai_addr) << std::endl;
    }
}

std::string Address::addr_to_string(const ::sockaddr *sockaddr) {
    if (sockaddr->sa_family == AF_UNSPEC)
        return "address family not specified";

    char buffer[INET6_ADDRSTRLEN] = {0};


    //NOTE might not work for IPv6
    if (nullptr != inet_ntop(sockaddr->sa_family,
                             (void *) &(((sockaddr_in *) sockaddr)->sin_addr),
                             buffer, sizeof(buffer))) {
        return buffer;
    }
    return strerror(errno);
}

std::shared_ptr<Address> Address::get_any_address(int port) {
    auto *socketaddr = new sockaddr_in{
        .sin_family = AF_INET,
        .sin_port = htons(port),
        .sin_addr =  {htonl(INADDR_ANY)},
    };

    return std::make_shared<Address>(reinterpret_cast<sockaddr *>(socketaddr), sizeof(sockaddr_in));
}

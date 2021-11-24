#include "connection.hpp"

#include <stdexcept>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <iostream>

Connection::Connection(const std::string &host_address) {
    file_descriptor = socket(AF_INET, SOCK_STREAM, 0);
    struct addrinfo hints{.ai_family = AF_UNSPEC, .ai_socktype = SOCK_STREAM};
    //todo error handling
    if (0 != getaddrinfo(host_address.c_str(), "http", &hints, &address_info)) {
        throw std::runtime_error("Could not get adressinfo of '" + host_address + "'");
    }
    if (0 != connect(file_descriptor, address_info->ai_addr, address_info->ai_addrlen)) {
        throw std::runtime_error("Could not connect to " + addrinfo_to_string(address_info));
    }
}

Connection::~Connection() {
    close(file_descriptor);
    freeaddrinfo(address_info);
}

std::string Connection::receive() const {
    constexpr int buf_size = 20 * 1024;
    char response_buffer[buf_size] = {0};

    recv(file_descriptor, response_buffer, buf_size, MSG_WAITALL);
    return response_buffer;
}

void Connection::send(const std::string &message) const {
    const char *string = message.c_str();
    ::send(file_descriptor, string, message.length(), 0);
}

void Connection::print_all_ip_info(addrinfo *result) {
    for (addrinfo *p = result; p != nullptr; p = p->ai_next) {
        std::cout << addrinfo_to_string(p) << std::endl;
    }
}

std::string Connection::addrinfo_to_string(const addrinfo *p) {
    char ipstr[INET6_ADDRSTRLEN] = {0};
    void *addr;

    if (p->ai_family == AF_INET) {
        auto *ipv4 = (struct sockaddr_in *) p->ai_addr;
        addr = &(ipv4->sin_addr);
    } else {
        auto *ipv6 = (struct sockaddr_in6 *) p->ai_addr;
        addr = &(ipv6->sin6_addr);
    }

    inet_ntop(p->ai_family, addr, ipstr, sizeof(ipstr));
    return ipstr;
}

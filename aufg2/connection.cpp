#include "connection.hpp"

#include <stdexcept>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>

Connection::Connection(const std::string& host_address) {
    file_descriptor = socket(AF_INET, SOCK_STREAM, 0);
    struct addrinfo hints{.ai_family = AF_UNSPEC, .ai_socktype = SOCK_STREAM};
    //todo error handling
    if (!getaddrinfo(host_address.c_str(), "http", &hints, &address_info)){
        throw std::runtime_error("Could not get adressinfo of '" + host_address + "'");
    }
    if (!connect(file_descriptor, address_info->ai_addr, address_info->ai_addrlen)){
        throw std::runtime_error("Could not connect");
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

void Connection::send(const std::string& message) const {
    const char *string = message.c_str();
    ::send(file_descriptor, string, message.length(), 0);
}

void Connection::print_ip_info(addrinfo *result) {
    char ipstr[INET6_ADDRSTRLEN] = {0};
    for (addrinfo *p = result; p != NULL; p = p->ai_next) {
        void *addr;
        char *ipver;
        // get the pointer to the address itself,
        // different fields in IPv4 and IPv6:
        if (p->ai_family == AF_INET) { // IPv4
            struct sockaddr_in *ipv4 = (struct sockaddr_in *) p->ai_addr;
            addr = &(ipv4->sin_addr);
            ipver = "IPv4";
        } else { // IPv6
            struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *) p->ai_addr;
            addr = &(ipv6->sin6_addr);
            ipver = "IPv6";
        }

        // convert the IP to a string and print it:
        inet_ntop(p->ai_family, addr, ipstr, sizeof(ipstr));
        printf("  %s: %s\n", ipver, ipstr);
    }
}

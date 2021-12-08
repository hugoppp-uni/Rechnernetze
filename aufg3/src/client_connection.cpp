#include "client_connection.hpp"

#include <iostream>
#include <cstring>
#include <arpa/inet.h>

ClientConnection::ClientConnection(struct sockaddr sockaddr, socklen_t socklen, int file_descriptor) :
    socket_address(sockaddr), socket_address_length(socklen), file_descriptor(file_descriptor) {}

ClientConnection::~ClientConnection() {
    if (0 != close(file_descriptor)) {
        std::cout << "could not close socket " << file_descriptor << " :" << strerror(errno);
    }
}

std::string ClientConnection::str() {
    auto *addr_in = (struct sockaddr_in *)&socket_address;
    char *s = inet_ntoa(addr_in->sin_addr);
    return s;
}

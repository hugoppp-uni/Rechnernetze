#include "client_connection.hpp"

#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <vector>
#include <array>

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

//TODO this method is copied from aufg2, we can probably just use the same class by merging the ctors
std::vector<char> ClientConnection::receive_bytes() const {
    std::vector<char> result_buffer;
    result_buffer.reserve(16 * 1024);

    size_t size;
    std::array<char, 2 * 1024> recv_buffer{0};
    while (0 < (size = recv(file_descriptor, &recv_buffer[0], recv_buffer.size(), 0))) {
        result_buffer.insert(result_buffer.end(), recv_buffer.begin(), recv_buffer.begin() + size);
    }

    if (size < 0) {
        std::cerr << "error occurred during receive_bytes: " << strerror(errno) << std::endl;
    }

    return result_buffer;
}

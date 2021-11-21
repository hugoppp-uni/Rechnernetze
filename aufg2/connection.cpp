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

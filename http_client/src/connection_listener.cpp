#include "connection_listener.hpp"

#include <sys/socket.h>
#include <memory>
#include <stdexcept>
#include <cstring>
#include <iostream>
#include <unistd.h>


ConnectionListener::ConnectionListener(int port) : port(port) {

    // When INADDR_ANY is specified in the bind call, the socket will be bound to all local interfaces
    auto server_addr = Address::get_any_address(port);

    file_descriptor = socket(AF_INET, SOCK_STREAM, 0);
    if (0 > file_descriptor) {
        std::cout << "Could not create socket: " << strerror(errno) << std::endl;
        exit(1);
    }
    if (0 != bind(file_descriptor, server_addr->get_sockaddr(), server_addr->get_socklen())) {
        std::cout << "Could not bind socket: " << strerror(errno) << std::endl;
        exit(1);
    }

    std::cout << "Listening on port " << port << std::endl;
}

ConnectionListener::~ConnectionListener() {
    if (0 != close(file_descriptor)) {
        std::cout << "Could not close socket " << file_descriptor << " :" << strerror(errno) << std::endl;
    } else {
        std::cout << "No longer listening on port " << port << std::endl;
    }
}

std::unique_ptr<Connection> ConnectionListener::accept_next_connection(int backlog) const {
    if (0 != ::listen(file_descriptor, backlog)) {
        std::cout << "Could not listen on socket: " << strerror(errno) << std::endl;
        return nullptr;
    }

    auto* peer_sockaddr = new struct sockaddr();

    socklen_t peer_socklen{};
    int peer_file_descriptor = ::accept(file_descriptor, peer_sockaddr, &peer_socklen);

    if (0 > peer_file_descriptor) {
        std::cout << "Could not accept connection: " << strerror(errno) << std::endl;
        return nullptr;
    }

    return std::make_unique<Connection>(std::make_shared<Address>(peer_sockaddr, peer_socklen), peer_file_descriptor);
}

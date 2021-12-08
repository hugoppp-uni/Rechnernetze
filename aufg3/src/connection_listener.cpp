#include "connection_listener.hpp"

#include <sys/socket.h>
#include <memory>
#include <stdexcept>
#include <cstring>
#include <iostream>
#include <unistd.h>


int listen(int sockfd, int backlog);


ConnectionListener::ConnectionListener(int port) : port(port) {

    auto server_address = get_server_address(port);

    file_descriptor = socket(AF_INET, SOCK_STREAM, 0);
    if (0 > file_descriptor) {
        std::cout << "Could not create socket: " << strerror(errno) << std::endl;
        exit(1);
    }
    if (0 != bind(file_descriptor, (struct sockaddr *) &server_address, sizeof(server_address))) {
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

std::unique_ptr<ClientConnection> ConnectionListener::accept_next_connection(int backlog) const {
    if (0 != ::listen(file_descriptor, backlog)) {
        std::cout << "Could not listen on socket: " << strerror(errno) << std::endl;
        return nullptr;
    }

    sockaddr peer_sockaddr{};
    socklen_t peer_socklen{};
    int peer_file_descriptor = ::accept(file_descriptor, &peer_sockaddr, &peer_socklen);

    if (0 > peer_file_descriptor) {
        std::cout << "Could not accept connection: " << strerror(errno) << std::endl;
        return nullptr;
    }

    return std::make_unique<ClientConnection>(peer_sockaddr, peer_socklen, peer_file_descriptor);
}

sockaddr_in ConnectionListener::get_server_address(int port) {
    return sockaddr_in{
        .sin_family = AF_INET,
        .sin_port = htons(port),
        .sin_addr =  {htonl(INADDR_ANY)},
    };
}

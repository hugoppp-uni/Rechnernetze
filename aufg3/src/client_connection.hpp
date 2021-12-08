#pragma once

#include <unistd.h>
#include <netdb.h>
#include <string>

class ClientConnection {
public:
    ClientConnection(sockaddr sockaddr, socklen_t socklen, int file_descriptor);

    virtual ~ClientConnection();


    std::string str();

private:
    struct sockaddr socket_address{};
    socklen_t socket_address_length{};
    int file_descriptor{};

};




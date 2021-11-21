#pragma once
#include <netdb.h>
#include <string>

class Connection{
public:
    Connection(const std::string &host_address);

    std::string receive() const;
    void send(const std::string& message) const;
    virtual ~Connection();

private:
    int file_descriptor{};
    addrinfo *address_info{};

    void print_ip_info(addrinfo *result);
};
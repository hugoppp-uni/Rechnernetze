#pragma once
#include <netdb.h>
#include <string>
#include <vector>

class Connection{
public:
    Connection(const std::string &host_address);

    std::vector<char> receive() const;
    void send(const std::string& message) const;
    virtual ~Connection();

private:
    int file_descriptor{};
    addrinfo *address_info{};

    void print_all_ip_info(addrinfo *result);

    static std::string addrinfo_to_string(const addrinfo *p) ;
};
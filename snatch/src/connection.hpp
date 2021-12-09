#pragma once
#include <netdb.h>
#include <string>
#include <vector>
#include "http_response.h"

class Connection{
public:
    Connection(const std::string &host_address);

    Connection(sockaddr *sockaddr, socklen_t socklen, int file_descriptor);

    std::string str();

    [[nodiscard]] std::vector<char> receive_bytes() const;
    [[nodiscard]] std::string receive_string() const;
    [[nodiscard]] HttpResponse receive_http_response() const;
    void send(const std::string& message) const;
    void send_slow(const std::string& message, int n_bytes, int timeout_ms) const;
    virtual ~Connection();

private:
    int file_descriptor{};

    addrinfo *address_info{};
    ::sockaddr *sockaddr_in_use{};
    socklen_t sockaddr_in_use_length;

    static void print_all_ip_info(addrinfo *result);


    static std::string addr_to_string(const ::sockaddr *sockaddr);
};
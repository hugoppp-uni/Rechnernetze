#pragma once

#include <netdb.h>
#include <iostream>

#include <string>

class Address {
public :
    explicit Address(const std::string &host_address) ;
    Address(struct sockaddr *sockaddr, socklen_t socklen);

    virtual ~Address();

    [[nodiscard]] const sockaddr* get_sockaddr() const;
    [[nodiscard]] socklen_t get_socklen() const;

    [[nodiscard]] std::string str() const;
    void print_all_ip_addresses() const;

    static std::shared_ptr<Address> get_any_address(int port);

private:
    addrinfo *address_info{};
    ::sockaddr *sockaddr_in_use{};
    socklen_t sockaddr_in_use_length;

    static std::string addr_to_string(const sockaddr *sockaddr);

};




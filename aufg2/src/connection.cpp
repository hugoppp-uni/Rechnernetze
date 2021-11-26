#include "connection.hpp"

#include <stdexcept>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <iostream>
#include <array>
#include <vector>
#include <cstring>
#include <chrono>
#include <thread>

Connection::Connection(const std::string &host_address) {
    file_descriptor = socket(AF_INET, SOCK_STREAM, 0);
    struct addrinfo hints{.ai_family = AF_UNSPEC, .ai_socktype = SOCK_STREAM};
    //todo error handling
    if (0 != getaddrinfo(host_address.c_str(), "http", &hints, &address_info)) {
        throw std::runtime_error("Could not get adressinfo of '" + host_address + "'");
    }
    if (0 != connect(file_descriptor, address_info->ai_addr, address_info->ai_addrlen)) {
        throw std::runtime_error("Could not connect to " + addrinfo_to_string(address_info));
    }
}

Connection::~Connection() {
    close(file_descriptor);
    freeaddrinfo(address_info);
}

std::vector<char> Connection::receive_bytes() const {
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

void Connection::send(const std::string &message) const {
    const char *string = message.c_str();
    ::send(file_descriptor, string, message.length(), 0);
}

void Connection::send_slow(const std::string &message, int n_bytes, int timeout_ms) const {
    std::cout << "Sending message in slow-motion mode (BYTES=" << n_bytes << ", TIMEOUT=" << timeout_ms << "ms)"
              << std::endl;
    // If the whole message should be sent at once, use normal send()-method
    if (n_bytes >= message.length()) {
        send(message);
    }

    int bytes_sent = 0;
    while (bytes_sent < message.length()) {
        if (bytes_sent + n_bytes >= message.length()) {
            n_bytes = (int) message.length() - bytes_sent;
        }
        ::send(file_descriptor, message.substr(bytes_sent, n_bytes).c_str(), n_bytes, 0);
        std::cout << "   Byte " << bytes_sent << " to " << bytes_sent + n_bytes - 1 << " were sent" << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(timeout_ms));
        bytes_sent += n_bytes;
    }
}

void Connection::print_all_ip_info(addrinfo *result) {
    for (addrinfo *p = result; p != nullptr; p = p->ai_next) {
        std::cout << addrinfo_to_string(p) << std::endl;
    }
}

std::string Connection::addrinfo_to_string(const addrinfo *p) {
    char ipstr[INET6_ADDRSTRLEN] = {0};
    void *addr;

    if (p->ai_family == AF_INET) {
        auto *ipv4 = (struct sockaddr_in *) p->ai_addr;
        addr = &(ipv4->sin_addr);
    } else {
        auto *ipv6 = (struct sockaddr_in6 *) p->ai_addr;
        addr = &(ipv6->sin6_addr);
    }

    inet_ntop(p->ai_family, addr, ipstr, sizeof(ipstr));
    return ipstr;
}

HttpResponse Connection::receive_http_response() const {
    return HttpResponse{receive_bytes()};
}

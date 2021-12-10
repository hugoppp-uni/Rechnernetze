#include "connection.hpp"

#include <stdexcept>
#include <unistd.h>
#include <arpa/inet.h>
#include <iostream>
#include <array>
#include <vector>
#include <cstring>
#include <chrono>
#include <thread>

Connection::Connection(const std::shared_ptr<Address> &address, int file_desciptor) : address(address),
                                                                                      file_descriptor(file_desciptor) {

}

Connection::Connection(const std::string& host) : Connection(std::make_shared<Address>(host)) {}

Connection::Connection(const std::shared_ptr<Address> &address) : address(address) {

    file_descriptor = socket(AF_INET, SOCK_STREAM, 0);
    if (0 != connect(file_descriptor, address->get_sockaddr(), address->get_socklen())) {
        throw std::runtime_error(
            "Could not connect to " + address->str());
    }

    std::cout << "available ip addresses:" << std::endl;
    address->print_all_ip_addresses();

    std::cout << "using ip address:" << std::endl;
    std::cout << address->str() << std::endl;
}


Connection::~Connection() {
    close(file_descriptor);

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


HttpResponse Connection::receive_http_response() const {
    return HttpResponse{receive_bytes()};
}

std::string Connection::receive_string() const {
    const std::vector<char> vector = receive_bytes();
    return {vector.begin(), vector.end()};
}

std::shared_ptr<Address> Connection::get_address() {
    return address;
}


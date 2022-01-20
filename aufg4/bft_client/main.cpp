#include <iostream>

#include "options.hpp"
#include "bft_datagram.hpp"

//c
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <filesystem>
#include <arpa/inet.h>
#include <fstream>
#include "logger.hpp"
#include "flags.hpp"

int sock_fd;
// TODO: Define timer

void send_datagram(const BftDatagram &datagram, sockaddr_in &server_addr);

int main(int argc, char **args) {
    Options options{argc, args};
    std::cout << "Hello Client" << std::endl;
    Logger::set_level(Logger::level::DATA);

    /* create an Internet, datagram, socket using UDP */
    sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock_fd == -1) {
        /* if socket failed to initialize, exit */
        printf("Error Creating Socket");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_addr{
        .sin_family = AF_INET,
        .sin_port = htons(options.server_port),
        .sin_addr = {inet_addr(options.server_ip.c_str())},
    };

    Logger::info("Uploading '" + options.file_path + "' to server " + options.server_ip);

    BftDatagram syn_datagram(Flags::SYN, std::filesystem::path(options.file_path).filename());
    send_datagram(syn_datagram, server_addr);

    std::array<char, MAX_PAYLOAD_SIZE> send_data = {};
    std::ifstream file{options.file_path, std::ios_base::in | std::ios::binary};
    while (true) {
        file.read(send_data.data(), send_data.size());
        long bytes_read = file.gcount();
        if (bytes_read <= 0)
            break;

        BftDatagram data_datagram = BftDatagram(Flags::None, send_data.begin(), send_data.begin() + bytes_read);
        send_datagram(data_datagram, server_addr);
    }

    send_datagram(BftDatagram(Flags::FIN), server_addr);

    file.close();

    close(sock_fd); /* close the socket */
    return 0;
}

void send_datagram(const BftDatagram &datagram, sockaddr_in &server_addr) {

    bool success = false;
    bool timeout = false;
    while (!success) {
        // TODO: Send packet with current SQN and start timer

        int bytes_send = datagram.send(sock_fd, server_addr);
        if (bytes_send < 0) {
            printf("Error sending packet: %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }

        BftDatagram response = BftDatagram::receive(sock_fd, server_addr);

        if (response.check_integrity() && (response.get_flags() & Flags::ACK) == Flags::ACK) { // TODO: also check SQN
            // TODO: Increase SQN
            success = true;
        } else if (timeout) { // TODO: check timeout correctly
            Logger::debug("Timeout occurred. Retransmit packet...");
            // TODO: Retransmit packet
            // TODO: Restart timer
        } else { // Packet is corrupt
            Logger::debug("Packet seems to be corrupt.");
            success = true;
        }
    }
}



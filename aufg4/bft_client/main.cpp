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

void send_datagram(const BftDatagram &datagram, sockaddr_in &server_addr, unsigned int recv_timeout_sec);

int sock_fd;
bool currSQN = false;

int main(int argc, char **args) {
    Options options{argc, args};

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
    unsigned int timeout_sec = options.retransmission_timeout_ms / 1000;
    BftDatagram syn_datagram(Flags::SYN , std::filesystem::path(options.file_path).filename(), currSQN);
    send_datagram(syn_datagram, server_addr, timeout_sec);

    std::array<char, MAX_PAYLOAD_SIZE> send_data = {};
    std::ifstream file{options.file_path, std::ios_base::in | std::ios::binary};

    while (true) {
        file.read(send_data.data(), send_data.size());
        long bytes_read = file.gcount();
        if (bytes_read <= 0)
            break;

        BftDatagram data_datagram = BftDatagram(Flags::None, send_data.begin(),
                                                send_data.begin() + bytes_read, currSQN);
        send_datagram(data_datagram, server_addr, timeout_sec);
    }

    send_datagram(BftDatagram(Flags::FIN, currSQN), server_addr, timeout_sec);

    file.close();

    close(sock_fd); /* close the socket */
    return 0;
}


void send_datagram(const BftDatagram &datagram, sockaddr_in &server_addr, unsigned int recv_timeout_sec) {

    while (true) {
        BftDatagram response;

        while (true) {
            int bytes_sent = datagram.send(sock_fd, server_addr);
            if (bytes_sent < 0) { // Exception occurred -> exit program
                perror("error during send");
                exit(EXIT_FAILURE);
            }
            int bytes_recvd = BftDatagram::receive(sock_fd, server_addr, response, recv_timeout_sec);
            if (bytes_recvd < 0 && errno == EAGAIN) { // Timeout occurred -> try again
                Logger::debug("Timeout occurred. Retransmit packet...");
            } else if (bytes_recvd < 0) { // Exception occurred -> exit program
                perror("error during receive");
                exit(EXIT_FAILURE);
            } else { // Packet was sent within timeout
                break;
            }
        }

        if (response.check_integrity()
            && (response.get_flags() & Flags::ACK) == Flags::ACK
            && (response.get_SQN() == currSQN)
                ) {
            currSQN ^= true;
            return;
        }

        Logger::debug("Packet seems to be corrupt, resending it");
    }
}



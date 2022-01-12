#include <iostream>

#include "options.hpp"
#include "bft_datagram.hpp"

//c
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "logger.hpp"

int sockfd;
// TODO: Define timer

void send_datagram(BftDatagram &datagram, sockaddr_in &server_addr);

void send_datagram(BftDatagram &datagram, sockaddr_in &server_addr) {
    socklen_t len = sizeof server_addr;

    bool success = false;
    bool timeout = false;
    while (!success) {
        // TODO: Send packet with current SQN and start timer
        Logger::debug("Sending packet with checksum " + datagram.checksum_as_string());
        ssize_t bytes_sent = sendto(sockfd, &datagram, datagram.size(), MSG_CONFIRM, (struct sockaddr *) &server_addr, sizeof server_addr);
        if (bytes_sent < 0) {
            printf("Error sending packet: %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
        Logger::debug("Waiting for response from server");
        int bytes_recvd = (int) recvfrom(sockfd, (void *) &datagram, MAX_DATAGRAM_SIZE, MSG_WAITALL, (struct sockaddr *) &server_addr, &len);
        if (bytes_recvd < 0) {
            perror("Error during receive");
            exit(EXIT_FAILURE);
        }
        Logger::debug("Bytes received: " + std::to_string(bytes_recvd));
        if (datagram.check_integrity() && (datagram.flags & Flags::ACK) == Flags::ACK) { // TODO: also check SQN
            Logger::debug("Received ACK");
            // TODO: Increase SQN
            success = true;
        } else if (timeout) { // TODO: check timeout correctly
            Logger::debug("Timeout occurred. Retransmit packet...");
            // TODO: Retransmit packet
            // TODO: Restart timer
        } else { // Packet is corrupt
            // TODO: what to do if integrity check of packet has failed?
            Logger::debug("Packet seems to be corrupt.");
            success = true;
        }
    }
}


int main(int argc, char **args) {
    Options options{argc, args};
    std::cout << "Hello Client" << std::endl;

    /* create an Internet, datagram, socket using UDP */
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == -1) {
        /* if socket failed to initialize, exit */
        printf("Error Creating Socket");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_addr {
        .sin_family = AF_INET,
        .sin_port = htons(options.server_port),
        .sin_addr = {inet_addr(options.server_ip.c_str())},
    };

    Logger::debug("Start Sync with server IP " + options.server_ip);
    BftDatagram datagram {
        .payload_size = static_cast<unsigned short>(options.file_path.size()),
        .flags = Flags::SYN,
    };
    options.file_path.copy(&datagram.payload[0], options.file_path.size());
    datagram.build();

    // TODO: while !EOF (File to be sent)
    send_datagram(datagram, server_addr);

    close(sockfd); /* close the socket */
    return 0;
}

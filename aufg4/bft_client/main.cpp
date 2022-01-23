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
#include <csignal>
#include "logger.hpp"
#include "flags.hpp"

void send_datagram(const BftDatagram &datagram, sockaddr_in &server_addr);

BftDatagram send_and_receive_response(const BftDatagram &datagram, sockaddr_in &server_addr, int max_retries = -1);

int sock_fd;
bool currSQN = false;
sockaddr_in server_addr;

unsigned int nRetransmissions;

void signalHandler(int signum) {
    Logger::info("Server shutting down, notifying server");

    BftDatagram abr_datagram(Flags::ABR, SQN_START_VAL);
    BftDatagram response;

    do {
        try {
            response = send_and_receive_response(abr_datagram, server_addr, 3);
        } catch (...) {
            Logger::warn("Could not inform server about shutdown");
            break;
        }
    } while (!response.check_integrity() || !response.is_ACK_for(abr_datagram));

    close(sock_fd);
    exit(EXIT_SUCCESS);
}


int main(int argc, char **args) {
    Options options{argc, args};

    if (options.debug)
        Logger::set_level(Logger::level::DEBUG);
    else
        Logger::set_level(Logger::level::INFO);

    /* create an Internet, datagram, socket using UDP */
    sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock_fd == -1) {
        /* if socket failed to initialize, exit */
        printf("Error Creating Socket");
        exit(EXIT_FAILURE);
    }

    server_addr = sockaddr_in{
        .sin_family = AF_INET,
        .sin_port = htons(options.server_port),
        .sin_addr = {inet_addr(options.server_ip.c_str())},
    };

    signal(SIGINT, signalHandler); // Cleanup when pressing CTRL+C

    Logger::debug("Setting retransmission timeout to " + std::to_string(options.retransmission_timeout_ms) + " ms.");
    int timeout_sec = options.retransmission_timeout_ms / 1000;
    int timeout_usec;
    if (timeout_sec == 0) {
        timeout_usec = options.retransmission_timeout_ms * 1000;
    } else {
        timeout_usec = (options.retransmission_timeout_ms % (timeout_sec * 1000)) * 1000;
    }
    struct timeval tv{
        .tv_sec = (time_t) timeout_sec,
        .tv_usec = (time_t) timeout_usec
    };
    setsockopt(sock_fd, SOL_SOCKET, SO_RCVTIMEO, (const char *) &tv, sizeof tv);

    Logger::info("Uploading '" + options.file_path + "' to server " + options.server_ip);
    nRetransmissions = 0;

    BftDatagram syn_datagram(Flags::SYN, std::filesystem::path(options.file_path).filename(), currSQN);
    send_datagram(syn_datagram, server_addr);

    std::array<char, MAX_PAYLOAD_SIZE> send_data = {};
    std::ifstream file{options.file_path, std::ios_base::in | std::ios::binary};
    while (true) {
        file.read(send_data.data(), send_data.size());
        long bytes_read = file.gcount();
        if (bytes_read <= 0)
            break;

        BftDatagram data_datagram = BftDatagram(Flags::None, send_data.begin(),
                                                send_data.begin() + bytes_read, currSQN);
        send_datagram(data_datagram, server_addr);
    }

    send_datagram(BftDatagram(Flags::FIN, currSQN), server_addr);

    file.close();

    Logger::info("Finished sending file to server.");
    Logger::debug("Number of retransmissions: " + std::to_string(nRetransmissions));

    close(sock_fd); /* close the socket */
    return 0;
}


void send_datagram(const BftDatagram &datagram, sockaddr_in &server_addr) {

    while (true) {
        BftDatagram response = send_and_receive_response(datagram, server_addr, -1);

        if (!response.check_integrity()) {
            Logger::warn("Response seems to be corrupt. Retransmitting packet...");
            continue;
        }
        //special case: if the server gets terminated, the server sends an ABR with no SQN set
        if ((response.get_flags() & Flags::ABR) == Flags::ABR && !response.get_SQN()) {
            Logger::error("Server busy, aborting upload");
            exit(EXIT_FAILURE);
        }
        if (response.get_SQN() != currSQN) {
            Logger::warn("Response SQN doesn't match. Retransmitting packet...");
            continue;
        }

        if ((response.is_ACK_for(datagram))) {
            currSQN ^= true;
            return;
        } else if ((response.get_flags() & Flags::AGN) == Flags::AGN) {
            Logger::warn("Server sent AGN, Retransmitting packet...");
        } else {
            Logger::warn("Server sent unexpected flags: '" + flags_to_str(response.get_flags())
                         + "' Retransmitting packet...");
        }
    }
}

BftDatagram send_and_receive_response(const BftDatagram &datagram, sockaddr_in &server_addr, int max_retries) {

    int retry_count = 0;
    while (true) {

        int bytes_sent = datagram.send(sock_fd, server_addr);
        if (bytes_sent < 0) { // Exception occurred -> exit program
            perror("error during send");
            exit(EXIT_FAILURE);
        }

        BftDatagram response;
        int bytes_recvd = BftDatagram::receive(sock_fd, server_addr, response);
        if (bytes_recvd > 0) {
            return response;
        }

        if (errno == EAGAIN) { // Timeout occurred -> try again

            if (max_retries < 0)
                Logger::info("Timeout occurred. Retransmitting packet...");
            else {
                if (retry_count++ < max_retries) {
                    Logger::info("Timeout occurred. Retransmitting packet... " + std::to_string(retry_count) + "/" + std::to_string(max_retries));
                } else {
                    throw std::runtime_error(
                        "Failed to send datagram after " + std::to_string(max_retries) + "retries");
                }
            }

            nRetransmissions++;
        } else {
            perror("error during receive");
            exit(EXIT_FAILURE);
        }

    }
}



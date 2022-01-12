#include "options.hpp"
#include "bft_datagram.hpp"
#include "file_writer.hpp"

//c
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h> /* for close() for socket */
#include "logger.hpp"
#include <csignal>

#include "lib/include/CRC.h"
#include <iomanip>  // Includes ::std::hex
#include <iostream> // Includes ::std::cout
#include <cstdint>  // Includes ::std::uint32_t

int sockfd;

BftDatagram handle_datagram(BftDatagram &datagram);
void receive_datagram(BftDatagram &datagram, sockaddr_in &client_addr);
bool check_datagram(BftDatagram &datagram);

void signalHandler(int signum) {
    Logger::debug("Interrupt signal " + std::to_string(signum) + " received.");
    // TODO: Cleanup stuff ...
    close(sockfd);
    exit(signum);
}

void receive_datagram(BftDatagram &datagram, sockaddr_in &client_addr){
    socklen_t len = sizeof client_addr;
    int n = (int) recvfrom(sockfd, (void *) &datagram, MAX_DATAGRAM_SIZE, MSG_WAITALL, (struct sockaddr *) &client_addr, &len);
    Logger::debug("Received new datagram (bytes recvd: " + std::to_string(n) + ")");
    std::string payload_str(datagram.payload.begin(), datagram.payload.end());
    Logger::debug("Received payload: " + payload_str + ", Payload Size: " + std::to_string(datagram.payload_size));
}

bool check_datagram(BftDatagram &datagram) {
    Logger::debug(datagram.checksum_as_string());
    if ( !datagram.check_integrity() ) {
        Logger::error("Datagram is not valid!");
        return false;
    }
    return true;
}

int main(int argc, char **args) {
    signal(SIGINT, signalHandler); // Cleanup when pressing CTRL+C

    Options options{argc, args};
    if (options.debug) {
        Logger::info("BFT Server started in Debug mode");
    } else {
        Logger::info("BFT Server started in Production mode");
    }
    Logger::info("Listening on port " + std::to_string(options.port));
    Logger::info("Incoming files will be saved into folder: " + options.directory);

    struct sockaddr_in server_addr {
        .sin_family = AF_INET,
        .sin_port = htons(options.port),
        .sin_addr =  {htonl(INADDR_ANY)}
    };

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    if ( bind(sockfd, (struct sockaddr *) &server_addr, sizeof server_addr ) == -1) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    BftDatagram datagram{0};
    sockaddr_in client_addr{0};
    while (true) {
        // 1. Receive packet
        receive_datagram(datagram, client_addr);
        BftDatagram response{0};
        if (check_datagram(datagram)){
            // 2. Packet ok
            Logger::debug("Packet ok. Handle it...");
            handle_datagram(datagram);
            Logger::debug("Handling done. Sending ACK.");
            response.flags = Flags::ACK;
        } else {
            // 3. Bit error
            Logger::debug("Packet not ok. Sending ERR");
            response.flags = Flags::ERR;
        }
        response.build();
        int bytes_sent = (int) sendto(sockfd, (void *) &response, MAX_DATAGRAM_SIZE, MSG_CONFIRM, (struct sockaddr *) &client_addr, sizeof client_addr);
        Logger::debug("ACK sent bytes: " + std::to_string(bytes_sent));
    }

}

static std::unique_ptr<FileWriter> fileWriter;

BftDatagram handle_datagram(BftDatagram &datagram) {
    BftDatagram response{0};
    if ((datagram.flags & Flags::SYN) == Flags::SYN) {
        Logger::debug("SYN flag is set -> Open file for writing with received payload as filename -> Set ACK flag");
        response.flags = Flags::ACK;
        std::string filename = std::string{datagram.payload.begin(), datagram.payload.begin() + datagram.payload_size};
        Logger::debug("Waiting for binary data of file: " + filename);
        fileWriter = std::make_unique<FileWriter>(filename);
        return response;
    } else if ((datagram.flags & Flags::ABR) == Flags::ABR) {
        Logger::debug("ABR flag is set -> Close opened file for writing and delete -> Set ACK flag");
        response.flags = Flags::ACK;
    } else {
        fileWriter->writeBytes(
                std::vector<unsigned char>(
                        datagram.payload.begin(),
                        datagram.payload.begin() + datagram.payload_size
                )
        );
    }

    return response;

}

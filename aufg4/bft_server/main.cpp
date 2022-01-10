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

void handle_datagram(const BftDatagram &datagram);

void signalHandler(int signum) {
    Logger::debug("Interrupt signal " + std::to_string(signum) + " received.");
    // TODO: Cleanup stuff ...
    exit(signum);
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

    struct sockaddr_in any_addr{
        .sin_family = AF_INET,
        .sin_port = htons(options.port),
        .sin_addr =  {htonl(INADDR_ANY)},
    };

    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (bind(sock, (struct sockaddr *) &any_addr, sizeof any_addr) == -1) {
        perror("Error bind failed");
        close(sock);
        exit(EXIT_FAILURE);
    }

    ssize_t recsize;
    BftDatagram datagram;

    for (;;) {

        sockaddr_in client_addr{0};
        socklen_t client_addr_len = 0;

        recsize = recvfrom(sock, (void *) &datagram, sizeof(BftDatagram), 0, (struct sockaddr *) &client_addr, &client_addr_len);
        if (recsize < 0) {
            perror("Error during receive");
            exit(EXIT_FAILURE);
        }

        handle_datagram(datagram);
    }

}

static std::unique_ptr<FileWriter> fileWriter;

void handle_datagram(BftDatagram &datagram) {
    Logger::info("Received new datagram");
    std::cout << "CRC32: " << std::hex << datagram.calc_checksum() << std::endl;
    if (datagram.calc_checksum() != datagram.checksum) {
        //send ERR
        std::cout << "CRC32 is not valid!" << std::endl;
        return;
    }

    if ((datagram.flags & Flags::SYN) == Flags::SYN) {
        //todo check checksum
        std::string filename = datagram.Payload();
        Logger::debug("Receiving file: " + filename);
        fileWriter = std::make_unique<FileWriter>(filename);
    }

/*    fileWriter->writeBytes(
        std::vector<unsigned char>(
            datagram.payload.begin(),
            datagram.payload.begin() + datagram.payload_size
        )
    );*/


}

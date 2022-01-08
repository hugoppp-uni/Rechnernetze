#include "options.hpp"
#include "bft_datagram.hpp"
#include "file_writer.hpp"

//c
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h> /* for close() for socket */

void handle_datagram(const BftDatagram &datagram);


int main(int argc, char **args) {

    Options options{argc, args};

    struct sockaddr_in any_addr{
        .sin_family = AF_INET,
        .sin_port = htons(options.port),
        .sin_addr =  {htonl(INADDR_ANY)},
    };

    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (bind(sock, (struct sockaddr *) &any_addr, sizeof any_addr) == -1) {
        perror("error bind failed");
        close(sock);
        exit(EXIT_FAILURE);
    }

    ssize_t recsize;
    BftDatagram diagram{0};

    for (;;) {

        sockaddr_in client_addr{0};
        socklen_t client_addr_len = 0;

        recsize = recvfrom(sock, (void *) &diagram, sizeof(BftDatagram), 0, (struct sockaddr *) &client_addr, &client_addr_len);
        if (recsize < 0) {
            fprintf(stderr, "%s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }

        handle_datagram(diagram);
    }

}

static std::unique_ptr<FileWriter> fileWriter;

void handle_datagram(const BftDatagram &datagram) {
    if (datagram.calc_checksum() != datagram.checksum) {
        //send ERR
        return;
    }

    if ((datagram.flags & Flags::SYN) == Flags::SYN) {
        //todo check checksum
        std::string filename = std::string{datagram.payload.begin(), datagram.payload.begin() + datagram.payload_size};
        fileWriter = std::make_unique<FileWriter>(filename);
    }

    fileWriter->writeBytes(
        std::vector<unsigned char>(
            datagram.payload.begin(),
            datagram.payload.begin() + datagram.payload_size
        )
    );


}

#include <iostream>

#include "options.hpp"
#include "bft_datagram.hpp"

//c
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>


int main(int argc, char **args) {
    Options options{argc, args};
    std::cout << "Hello Client" << std::endl;

    /* create an Internet, datagram, socket using UDP */
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == -1) {
        /* if socket failed to initialize, exit */
        printf("Error Creating Socket");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in sa{
        .sin_family = AF_INET,
        .sin_port = htons(options.server_port),
        .sin_addr = {inet_addr(options.server_ip.c_str())},
    };

    BftDatagram bftDatagram{
        .checksum = 0,
        .payload_size = static_cast<unsigned short>(options.file_path.size()),
        .flags = Flags::SYN,
        .payload = {0},
    };

    options.file_path.copy(&bftDatagram.payload[0], options.file_path.size());

    int bytes_sent = sendto(sock, &bftDatagram, bftDatagram.size(), 0, (struct sockaddr *) &sa, sizeof sa);

    if (bytes_sent < 0) {
        printf("Error sending packet: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    close(sock); /* close the socket */
    return 0;
}

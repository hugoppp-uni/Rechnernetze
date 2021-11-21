#include "options.hpp"
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>

void print_ip_info(addrinfo *result);

int main(int argc, char **argv) {
    Options opt{argc, argv};

    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    addrinfo hints{.ai_family = AF_UNSPEC, .ai_socktype = SOCK_STREAM};
    addrinfo *result;

    std::cout << opt.url << std::endl;

    getaddrinfo("www.scimbe.de", "http", &hints, &result);
    print_ip_info(result);

    connect(socket_fd, result->ai_addr, result->ai_addrlen);

    char request_buffer[] =
        "GET /_index.html HTTP/1.1\r\n"
        "User-Agent: Wget/1.20.3 (linux-gnu)\r\n"
        "Range: bytes=0-2234\r\n"
        "Accept: */*\r\n"
        "Host: scimbe.de\r\n"
        "\r\n\r\n";

    send(socket_fd, request_buffer, sizeof(request_buffer), 0);

    int size = 14 * 1024;
    char response_buffer[size];
    recv(socket_fd, response_buffer, size, MSG_WAITALL);
    std::cout << response_buffer << std::endl;
    close(socket_fd);

}

void print_ip_info(addrinfo *result) {
    char ipstr[INET6_ADDRSTRLEN] = {0};
    for (addrinfo *p = result; p != NULL; p = p->ai_next) {
        void *addr;
        char *ipver;
        // get the pointer to the address itself,
        // different fields in IPv4 and IPv6:
        if (p->ai_family == AF_INET) { // IPv4
            struct sockaddr_in *ipv4 = (struct sockaddr_in *) p->ai_addr;
            addr = &(ipv4->sin_addr);
            ipver = "IPv4";
        } else { // IPv6
            struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *) p->ai_addr;
            addr = &(ipv6->sin6_addr);
            ipver = "IPv6";
        }

        // convert the IP to a string and print it:
        inet_ntop(p->ai_family, addr, ipstr, sizeof(ipstr));
        printf("  %s: %s\n", ipver, ipstr);
    }
}

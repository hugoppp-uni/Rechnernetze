#include "options.hpp"
#include "connection.hpp"
#include <netdb.h>

int main(int argc, char **argv) {

    Options opt{argc, argv};

    Connection cnn{"www.scimbe.de"};
    std::string request =
        "GET /_index.html HTTP/1.1\r\n"
        "User-Agent: Wget/1.20.3 (linux-gnu)\r\n"
        "Range: bytes=0-2234\r\n"
        "Accept: */*\r\n"
        "Host: scimbe.de\r\n"
        "\r\n\r\n";

    cnn.send(request);
    std::cout << cnn.receive();
}

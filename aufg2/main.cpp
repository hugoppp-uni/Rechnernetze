#include "options.hpp"
#include "connection.hpp"
#include "http_request_builder.h"

int main(int argc, char **argv) {

    Options opt{argc, argv};

    auto url_info = UrlParser::parse(opt.url);

    HttpRequestBuilder request{url_info};
    request.add("Accept", "*/*");
    if (!opt.range.empty())
        request.add_range(opt.range);

    Connection cnn{url_info.host};

    const std::string message = request.to_string();
    if (opt.verbose)
        std::cout << message << std::endl;
    cnn.send(message);
    std::cout << cnn.receive();
}

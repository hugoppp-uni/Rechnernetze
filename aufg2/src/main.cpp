#include "options.hpp"
#include "connection.hpp"
#include "http_request_builder.h"
#include "Response.h"

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
        std::cerr << "\n***** REQUEST METADATA *****\n" << message << std::endl;

    cnn.send(message);

    std::string response_str = cnn.receive();
    Response response = Response(response_str);

    if (opt.verbose)
        std::cerr << "\n***** RESPONSE METADATA *****\n" << response.GetMetadata();

    std::cout << "\n***** RESPONSE PAYLOAD *****\n" << response.GetPayload();
}

#include "options.hpp"
#include "connection.hpp"
#include "http_request_builder.h"
#include "http_response.h"
#include <fstream>

void send_request(const Connection &cnn, const std::string &message, const Options &options);

std::string build_request(const UrlInfo &url_info, const Options &opt);

/*
 * URL's for testing:
 * http://scimbe.de/_index.html
 * http://www.columbia.edu/~fdc/picture-of-something.jpg
 * http://m.psecn.photoshelter.com/img-get/I0000jS0oRYHLEQw/s/1200/I0000jS0oRYHLEQw.jpg
 */
int main(int argc, char **argv) {

    Options opt{argc, argv};

    auto url_info = UrlParser::parse(opt.url);
    std::string message = build_request(url_info, opt);

    if (opt.verbose)
        std::cerr << "\n***** REQUEST METADATA *****\n" << message << std::endl;

    Connection cnn{url_info.host};
    send_request(cnn, message, opt);
    HttpResponse response = cnn.receive_http_response();

    if (opt.verbose)
        std::cerr << "\n***** RESPONSE METADATA *****\n" << response.get_metadata() << std::endl;

    std::string filename = !opt.output.empty() ? opt.output : url_info.file_name;
    if (!filename.empty() && !response.write_to_file(filename)) {
        return EXIT_FAILURE;
    }

}

std::string build_request(const UrlInfo &url_info, const Options &opt) {
    HttpRequestBuilder request{url_info};
    request.add("Accept", "*/*");
    request.add("User-Agent", "snatch/0.1.0");
    if (!opt.range.empty())
        request.add_range(opt.range);
    const std::string message = request.to_string();
    return message;
}

void send_request(const Connection &cnn, const std::string &message, const Options &options) {
    if (options.slow.has_value()) {
        auto slow_info = options.slow.value();
        cnn.send_slow(message, slow_info.bytes, slow_info.timeout);
    } else {
        cnn.send(message);
    }
}

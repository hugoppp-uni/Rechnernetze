#include "options.hpp"
#include "connection.hpp"
#include "http_request_builder.h"
#include "Response.h"
#include <fstream>

int main(int argc, char **argv) {

    Options opt{argc, argv};

    auto url_info = UrlParser::parse(opt.url);

    HttpRequestBuilder request{url_info};
    request.add("Accept", "*/*");
    request.add("User-Agent", "snatch/0.1.0");
    if (!opt.range.empty())
        request.add_range(opt.range);

    Connection cnn{url_info.host};

    const std::string message = request.to_string();
    if (opt.verbose)
        std::cerr << "\n***** REQUEST METADATA *****\n" << message << std::endl;

    if (opt.slow) {
        // TODO: If option --slow was set, call this function with provided parameters (number of bytes per send() and timeout in ms)
        cnn.send_slow(message, 5, 1000);
    } else {
        cnn.send(message);
    }

    std::vector<char> receive_data = cnn.receive();
    Response response = Response(receive_data);

    if (opt.verbose)
        std::cerr << "\n***** RESPONSE METADATA *****\n" << response.GetMetadata() << std::endl;

    /*
     * Write payload of received response to file, if option -o or -O was provided
     *
     * URL's for testing:
     * http://scimbe.de/_index.html
     * http://www.columbia.edu/~fdc/picture-of-something.jpg
     * http://m.psecn.photoshelter.com/img-get/I0000jS0oRYHLEQw/s/1200/I0000jS0oRYHLEQw.jpg
     */
    if (!opt.output.empty() || opt.remote_name) {
        std::string filename;
        if (!opt.output.empty()){
            filename = opt.output;
        } else {
            filename = url_info.file;
        }

        std::ofstream output_file{filename, std::ios::binary};
        if (!output_file){
            std::cerr << "Error opening file '" + filename + "' for writing response data to..." << std::endl;
            return EXIT_FAILURE;
        }

        for (char c : response.GetPayloadAsBinary()) {
            output_file << c;
        }

        std::cout << "Payload of reponse was written to file: " + filename << std::endl;
        output_file.close();
    }

}

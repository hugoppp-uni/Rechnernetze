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

    cnn.send(message);

    std::vector<char> receive_data = cnn.receive();

    // Write payload of received response to file, if option -o or -O was provided
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
        for (char c : receive_data) {
            output_file << c;
        }
        output_file.close();
    }

    std::string response_str = std::string(receive_data.begin(), receive_data.end());
    Response response = Response(response_str);

    if (opt.verbose)
        std::cerr << "\n***** RESPONSE METADATA *****\n" << response.GetMetadata();

    std::cout << "\n***** RESPONSE PAYLOAD *****\n" << response.GetPayload();
}

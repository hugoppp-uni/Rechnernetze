#include "request_handler.h"
#include <HttpRequest.h>
#include <HttpResponse.hpp>
#include <connection.hpp>

int handle_request(const std::unique_ptr<Connection>& cnn, HttpRequest *request) {

    HttpResponse response;

    // Check if method is supported (currently only GET)
    if (request->get_method() != HttpRequest::GET) {
        response.set_status(HttpResponse::Status::METHOD_NOT_ALLOWED);
        response.add_header("Content-Type", "text/plain");
        response.set_content("Sorry, this method is not allowed. Currently only GET is supported.");
        return EXIT_FAILURE;
    }

    std::string fpath = request->get_uri();
    // TODO: Set content of Response according to requested file/directory
    // 1) if fpath has file-ending: Set Content-Type inside header according to file-type, set Payload of Response to file content. If file is not existent, set according Status-Code
    // 2) if fpath is a directory: check if index.html is present inside this directory. If not, set Payload to directory listing as table. If yes, set Payload to index.html content.

    // TODO: WIP
    if ( !fpath.empty() ) {
        response.set_status(HttpResponse::Status::OK);
        // TODO: Set data for response (content of requested file/directory)
        std::vector<char> data = {'d', 'a', 't', 'a'};
        response.set_content(data);
    } else {
        response.set_status(HttpResponse::Status::BAD_REQUEST);
        response.add_header("Content-Type", "text/plain");
        response.set_content("Please provide a correct file path");
    }

    // Send response
    cnn->send(response.build());
//    cnn->send("HTTP/1.1 200 OK\r\nsomedata\r\n");

    return EXIT_SUCCESS;
}

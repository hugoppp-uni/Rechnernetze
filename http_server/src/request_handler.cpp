#include "request_handler.h"
#include <HttpRequest.h>
#include <HttpResponse.hpp>
#include <connection.hpp>
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;
std::string DOCUMENT_ROOT_FOLDER;

int handle_request(const std::unique_ptr<Connection>& cnn, HttpRequest *request) {

    HttpResponse response;

    // Check if method is supported (currently only GET)
    if (request->get_method() != HttpRequest::GET) {
        response.set_status(HttpResponse::Status::METHOD_NOT_ALLOWED);
        response.add_header("Content-Type", "text/plain");
        response.set_content("Sorry, this method is not allowed. Currently only GET is supported.");
        cnn->send(response.build());
        return EXIT_FAILURE;
    }

    // TODO: Set content of Response according to requested file/directory
    // 1) if fpath has file-ending: Set Content-Type inside header according to file-type, set Payload of Response to file content. If file is not existent, set according Status-Code
    // 2) if fpath is a directory: check if index.html is present inside this directory. If not, set Payload to directory listing as table. If yes, set Payload to index.html content.
    // Tipp: opendir(), closedir(), readdir() und stat()

    fs::path path = DOCUMENT_ROOT_FOLDER + request->get_uri();
    if ( is_directory(path) ) {
        std::cout << path.string() << " is a directory and exists" << std::endl;
        fs::path fpath = path.string() + "/index.html";
        if(exists(fpath)) {
            // index.html vorhanden: Sende Datei
            std::ifstream inf{fpath, std::ios::in | std::ios::binary};
            std::vector<char> result;
            // TODO: Am Ende wird ein Zeichen zu viel eingefügt
            while( !inf.eof() ) {
                result.insert(result.end(), (char) inf.get());
            }
            inf.close();
            response.add_header("Content-Type", "text/html");
            response.set_content(result);
        } else {
            // index.html vorhanden: Sende Listing der Dateien im Ordner
            std::string file_listing;
            file_listing.append("Directory content of " + request->get_uri() + ":\n");
            for (const auto & file : std::filesystem::directory_iterator(path)) {
                std::cout << file.path() << " | " << file.file_size() << std::endl;
                file_listing.append(file.path().string() + "\n");
            }
            response.add_header("Content-Type", "text/plain");
            response.set_content(file_listing);
        }
        response.set_status(HttpResponse::Status::OK);
    } else if(exists(path)){
        // File existiert: Content zurückliefern
        std::cout << path.string() << " is a file and exists" << std::endl;
        std::ifstream inf{path, std::ios::in | std::ios::binary};
        std::vector<char> result;
        // TODO: Am Ende wird ein Zeichen zu viel eingefügt
        while( !inf.eof() ) {
            result.insert(result.end(), (char) inf.get());
        }
        inf.close();
        response.add_header("Content-Type", "text/plain");
        response.set_content(result);
        response.set_status(HttpResponse::Status::OK);
    } else {
        // File existiert nicht: Directory Listing
        std::cout << path.string() << " does not exist -> Get directory listing" << std::endl;
        response.add_header("Content-Type", "text/plain");
        response.set_content("File " + path.string() + " does not exist. Please provide a correct file path");
        response.set_status(HttpResponse::Status::BAD_REQUEST);
    }

    // Send response
    cnn->send(response.build());

    return EXIT_SUCCESS;
}

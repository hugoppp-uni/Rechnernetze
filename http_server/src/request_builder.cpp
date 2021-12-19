#include "request_builder.h"
#include <HttpRequest.h>
#include <HttpResponse.hpp>
#include <filesystem>
#include "logger.hpp"
#include "helper.hpp"

#define FMT_HEADER_ONLY

#include <fmt/format.h>

namespace fs = std::filesystem;

std::string ResponseBuilder::build(const HttpRequest &request,
                                   const std::string &documents_root) {

    HttpResponse response;
    // Check if method is supported (currently only GET)
    if (request.get_method() != HttpRequest::GET) {
        std::string reason{"Sorry, this method is not allowed. Currently only GET is supported."};
        build_from_plain_text(response, HttpResponse::METHOD_NOT_ALLOWED, reason);
        return response.build();
    }

    // TODO: Set content of Response according to requested file/directory
    // 1) if fpath has file-ending: Set Content-Type inside header according to file-type, set Payload of Response to file content. If file is not existent, set according Status-Code
    // 2) if fpath is a directory: check if index.html is present inside this directory. If not, set Payload to directory listing as table. If yes, set Payload to index.html content.
    // Tipp: opendir(), closedir(), readdir() und stat()

    fs::path path = documents_root + request.get_uri();
    if (is_directory(path)) {
        Logger::info(path.string() + " is a directory and exists");
        build_from_directory(response, request, path);
    } else if (exists(path)) {
        Logger::info(path.string() + " is a file and exists");
        build_from_file(response, path);
    } else {
        Logger::error("Requested file " + path.string() + " does not exist.");
        std::string reason{"File '" + path.string() + "' does not exist. Please provide a correct file path"};
        build_from_plain_text(response, HttpResponse::BAD_REQUEST, reason);
    }

    return response.build();
}

void ResponseBuilder::build_from_plain_text(HttpResponse &response, HttpResponse::Status status, std::string &text) {
    response.add_header("Content-Type", "text/plain");
    response.set_content(text);
    response.set_status(status);
}

void ResponseBuilder::build_from_directory(HttpResponse &response,
                                           const HttpRequest &request,
                                           const fs::path &dir_path) {

    response.set_status(HttpResponse::OK);
    fs::path index_html_path = dir_path / "index.html";

    if (exists(index_html_path)) {
        // index.html vorhanden: Sende Datei
        Logger::info("index.html exists");
        build_from_file(response, index_html_path);
    } else {
        // index.html nicht vorhanden: Sende Listing der Dateien im Ordner
        Logger::warn("index.html does not exist. List directory content...");

        const std::string file_listing = "Directory content of " + request.get_uri() + '\n'
                                         + get_plain_text_file_listing(dir_path);

        response.add_header("Content-Type", "text/plain");
        response.set_content(file_listing);
    }
}

std::string ResponseBuilder::get_plain_text_file_listing(const fs::path &dir_path) {

    constexpr char *table_format = "| {:<50} | {:<30} | {:<30}|";
    constexpr int table_format_lengt = 1 + 50 + 3 + 30 + 3 + 30;
    std::string header{fmt::format(table_format, "filename", "size", "last modified") + '\n'};
    std::string separator{"|" + std::string(table_format_lengt, '-') + "|\n"};

    std::stringstream file_listing{};
    file_listing << separator
                 << header
                 << separator;

    for (const auto &file: std::filesystem::directory_iterator(dir_path)) {
        file_listing << fmt::format(table_format,
                                    file.path().filename().string(),
                                    helper::file_size_to_str(file.file_size()),
                                    "(TODO last write time)"
        );
        file_listing << '\n';
    }
    file_listing << separator;

    return file_listing.str();
}

void ResponseBuilder::build_from_file(HttpResponse &response,
                                      const fs::path &path) {

    response.add_header("Content-Type", "text/plain");
    response.set_content(helper::read_file(path));
    response.set_status(HttpResponse::OK);
}
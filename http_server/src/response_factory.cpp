#include "response_factory.h"
#include <HttpRequest.h>
#include <HttpResponse.hpp>
#include <filesystem>
#include "logger.hpp"
#include "helper.hpp"
#include <map>
#include <fmt/format.h>
#include <chrono>

namespace fs = std::filesystem;
using namespace std::chrono_literals;

HttpResponse ResponseFactory::create(const HttpRequest &request,
                                    const std::string &documents_root) {

    HttpResponse response;
    // Check if method is supported (currently only GET)
    if (request.get_method() != HttpRequest::Method::GET) {
        std::string reason{"Sorry, this method is not allowed. Currently only GET is supported."};
        build_from_plain_text(response, HttpResponse::Status::METHOD_NOT_ALLOWED, reason);
        return response;
    }

    // TODO: Set content of Response according to requested file/directory
    // 1) if fpath has file-ending: Set Content-Type inside header according to file-type, set Payload of Response to file content. If file is not existent, set according Status-Code
    // 2) if fpath is a directory: check if index.html is present inside this directory. If not, set Payload to directory listing as table. If yes, set Payload to index.html content.
    // Tipp: opendir(), closedir(), readdir() und stat()

    const std::string &uri = request.get_uri();
    fs::path path = documents_root + uri;
    if (is_directory(path)) {
        build_from_directory(response, request, path);
    } else if (exists(path)) {
        build_from_file(response, path);
    } else {
        std::string reason{fmt::format("Requested file {} does not exist. Please provide a correct file path ", uri)};
        build_from_plain_text(response, HttpResponse::Status::BAD_REQUEST, reason);
    }

    return response;
}

void ResponseFactory::build_from_plain_text(HttpResponse &response, HttpResponse::Status status, std::string &text) {
    response.add_header("Content-Type", "text/plain");
    response.set_content(text);
    response.set_status(status);
}

void ResponseFactory::build_from_directory(HttpResponse &response,
                                           const HttpRequest &request,
                                           const fs::path &dir_path) {

    response.set_status(HttpResponse::Status::OK);
    fs::path index_html_path = dir_path / "index.html";

    if (exists(index_html_path)) {
        // index.html vorhanden: Sende Datei
        build_from_file(response, index_html_path);
    } else {
        // index.html nicht vorhanden: Sende Listing der Dateien im Ordner

        const std::string file_listing = fmt::format("Directory content of '{}': \n{}",
                                                     request.get_uri(),
                                                     get_plain_text_file_listing(dir_path));

        response.add_header("Content-Type", "text/plain");
        response.set_content(file_listing);
    }
}

std::string ResponseFactory::get_plain_text_file_listing(const fs::path &dir_path) {

    constexpr char *table_format = "| {:<50} | {:>10} | {:<20}|";
    constexpr int table_format_lengt = 1 + 50 + 3 + 10 + 3 + 20;
    std::string header{fmt::format(table_format, "filename", "size", "last modified") + '\n'};
    std::string separator{"|" + std::string(table_format_lengt, '-') + "|\n"};

    std::stringstream file_listing{};
    file_listing << separator
                 << header
                 << separator;

    for (const auto &file: std::filesystem::directory_iterator(dir_path)) {

        const std::string &filesize = is_directory(file) ? "" :
                                      helper::file_size_to_str(file.file_size());
        const std::string &args = file.path().filename().string() + (is_directory(file) ? "/" : "");


        std::error_code error;
        fs::file_time_type file_time_point = file.last_write_time(error);

        std::time_t tt = helper::to_time_t(file_time_point);
        std::string formattedFileTime = helper::time_t_to_string(tt, "%F %T");

        const std::string &last_modified = error ? error.message() : formattedFileTime;

        file_listing << fmt::format(table_format,
                                    args,
                                    filesize,
                                    last_modified
        );
        file_listing << '\n';
    }
    file_listing << separator;

    return file_listing.str();
}

void ResponseFactory::build_from_file(HttpResponse &response,
                                      const fs::path &file_path) {

    std::string content_type = get_content_type(file_path);
    response.add_header("Content-Type", content_type);
    response.set_content(helper::read_file(file_path));
    response.set_status(HttpResponse::Status::OK);
}

std::string ResponseFactory::get_content_type(const fs::path &file_path) {

    std::string content_type;
    if (!file_path.has_extension())
        content_type = "text/plain";
    else {
        content_type = content_type_map[file_path.extension().string()];
        if (content_type.empty())
            content_type = "text/plain";
    }

    return content_type;
}


using ContentTypeMap = std::map<std::string, std::string>;
ContentTypeMap ResponseFactory::content_type_map = ContentTypeMap{
    {".txt",  "text/plain"},
    {".html", "text/html"},
    {".xml",  "text/xml"},

    {".jpeg", "image/jpeg"},
    {".png",  "image/png"},
    {".svg",  "image/svg+xml"},
    {".webp", "image/webp"},

    {".mp4",  "video/mp4"},

    {".pdf",  "application/pdf"},
};


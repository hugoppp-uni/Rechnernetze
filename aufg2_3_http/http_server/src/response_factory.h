#pragma once

#include <HttpRequest.h>
#include <memory>
#include <connection.hpp>
#include <filesystem>
#include <map>


class ResponseFactory {
public:
    static HttpResponse create(const HttpRequest &request,
                               const std::string &documents_root,
                               std::string &log);

    static HttpResponse create_from_plain_text(HttpResponse::Status status, const std::string &text);

private:
    static std::map<std::string, std::string> content_type_map;

    static HttpResponse create_from_file(const std::filesystem::path &file_path);

    static HttpResponse
    create_from_directory(const std::filesystem::path &dir_path, const std::string &uri, std::string &log);


    static std::string get_plain_text_file_listing(const std::filesystem::path &dir_path);

    static std::string get_content_type(const std::filesystem::path &file_path);
};

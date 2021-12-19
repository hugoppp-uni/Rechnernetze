#pragma once

#include <HttpRequest.h>
#include <memory>
#include <connection.hpp>
#include <filesystem>
#include <map>


class ResponseFactory {
public:
    static HttpResponse create(const HttpRequest &request, const std::string &documents_root);

private:
    static std::map<std::string, std::string> content_type_map;

    static void build_from_file(HttpResponse &response, const std::filesystem::path &file_path);

    static void
    build_from_directory(HttpResponse &response, const HttpRequest &request, const std::filesystem::path &dir_path);

    static void
    build_from_plain_text(HttpResponse &response, HttpResponse::Status status, std::string &text);

    static std::string get_plain_text_file_listing(const std::filesystem::path &dir_path);

    static std::string get_content_type(const std::filesystem::path &file_path);
};

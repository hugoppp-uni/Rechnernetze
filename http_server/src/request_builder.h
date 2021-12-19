#pragma once

#include <HttpRequest.h>
#include <memory>
#include <connection.hpp>
#include <filesystem>


class ResponseBuilder {
public:
    static std::string build(const HttpRequest &request, const std::string &document_root);

private:
    static void build_from_file(HttpResponse &response, const std::filesystem::path &path);

    static void
    build_from_directory(HttpResponse &response, const HttpRequest &request, const std::filesystem::path &dir_path);

    static void
    build_from_plain_text(HttpResponse &response, HttpResponse::Status status, std::string &text);

    static std::string get_file_listing(const std::filesystem::path &dir_path);
};

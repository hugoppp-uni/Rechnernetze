#pragma once

#include <string>
#include <vector>

class HttpResponse {
public:
    HttpResponse(const std::vector<char>& data);
    std::string get_metadata();

    std::vector<char> get_payload_as_binary();
    std::string get_payload_as_string();
    bool write_to_file(const std::string& filename);

    HttpResponse(const HttpResponse&) = delete;
    HttpResponse& operator=(const HttpResponse&) = delete;

private:
    std::string header;
    std::vector<char> payload;
};

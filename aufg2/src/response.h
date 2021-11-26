#pragma once

#include <string>
#include <vector>

class Response {
public:
    Response(const std::vector<char>& data);
    std::string get_metadata();

    std::vector<char> get_payload_as_binary();
    bool write_to_file(const std::string& filename);

private:
    std::string headers;
    std::vector<char> payload;

};

#pragma once

#include <string>
#include <vector>
#include <sstream>
#include "../../http_server/src/request_handler.h"

class HttpResponse {
public:
    enum Status {
        INVALID_CODE = 0,
        OK = 200,
        ACCEPTED = 202,
        PARTIAL_CONTENT = 206,
        MULTIPLE_CHOICES = 300,
        MOVED_PERMANENTLY = 301,
        BAD_REQUEST = 400,
        UNAUTHORIZED = 401,
        FORBIDDEN = 403,
        NOT_FOUND = 404,
        METHOD_NOT_ALLOWED = 405,
        NOT_ACCEPTABLE = 406,
        REQUEST_TIMEOUT = 408,
        UNSUPPORTED_MEDIA_TYPE = 415,
        INTERNAL_SERVER_ERROR = 500,
        NOT_IMPLEMENTED = 501,
        SERVICE_UNAVAILABLE = 503,
        VERSION_NOT_SUPPORTED = 505,
    };

    // Build Response from incoming data
    explicit HttpResponse(const std::vector<char>& data);
    std::string get_metadata();
    std::vector<char> get_payload_as_binary();
    std::string get_payload_as_string();
    bool write_to_file(const std::string& filename);

    // Build HTTP Response for sending
    HttpResponse();
    static std::string get_status_text(Status &status);
    void add_header(const std::string& key, const std::string& value);
    void set_status(Status s);
    void set_content(const std::vector<char>& data);
    std::string build();

private:
    Status status = INVALID_CODE;
    std::string header;
    std::vector<char> payload;
    std::stringstream result_stream;
};

#include <iostream>
#include <sstream>
#include <cstring>
#include "request_handler.h"
#include "http_response.hpp"
#include "connection.hpp"

HttpRequest::HttpRequest(std::string const& data) {
    // TODO: Parse first line of data
    // HttpRequest-Line = Method SP HttpRequest-URI SP HTTP-Version CRLF
    method = GET;
    uri = "index.html";
}

HttpRequest::Method HttpRequest::get_method() {
    return method;
}

std::string HttpRequest::get_version() {
    return http_version;
}

std::string HttpRequest::get_uri() {
    return uri;
}

std::string HttpRequest::get_headers() {
    return headers;
}

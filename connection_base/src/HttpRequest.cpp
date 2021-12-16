#include <iostream>
#include <regex>
#include "HttpRequest.h"

HttpRequest::HttpRequest(std::string const& data) {
    // Parse first line of data
    // Http Request-Line = Method SP HttpRequest-URI SP HTTP-Version CRLF
    std::regex regex("(GET) (/.*)* (HTTP/1.1)\r");
    std::smatch matches;
    std::istringstream iss(data);
    std::string status_line;
    std::getline(iss, status_line);
    std::cout << "Received Status-Line: " << status_line << std::endl;
    std::regex_match(status_line, matches, regex);
    if (!std::regex_match(status_line, matches, regex) /*|| matches.size() != 3*/) {
        throw std::invalid_argument("Status-Line");
    }

    if (matches[1] == "GET")
        method = GET;
    uri = matches[2];
    http_version = matches[3];
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
#include <regex>
#include "HttpRequest.h"

HttpRequest::HttpRequest(std::string const &data) {
    std::istringstream iss(data);

    {
        std::string status_line;
        std::getline(iss, status_line);
        std::regex regex("(.*) (/.*)* (HTTP/1.1)\r");
        std::smatch matches;
        std::regex_match(status_line, matches, regex);
        if (!std::regex_match(status_line, matches, regex) /*|| matches.size() != 3*/) {
            throw std::invalid_argument("Failed to parse Status-Line: " + status_line);
        }

        method = matches[1] == "GET"
                 ? Method::GET
                 : Method::NONE;

        uri = matches[2];
        http_version = matches[3];
    }

    {
        std::string header_line;
        while (std::getline(iss, header_line)) {
            std::regex regex("(.+?):\\s?(.+)\r");
            std::smatch matches;
            std::regex_match(header_line, matches, regex);
            if (std::regex_match(header_line, matches, regex)) {
                headers[matches[1]] = matches[2];
            }
        }
    }

    {
        if (headers.contains("Content-Range")) {
            std::string range_string{headers["Content-Range"]};
            std::regex regex("(\\d+)-(\\d*)");
            std::smatch matches;
            std::regex_match(range_string, matches, regex);
            if (std::regex_match(range_string, matches, regex) && matches.size() == 3) {
                int start{std::stoi(matches[1])};
                range = Range{.start = start};

                if (matches[2].length() > 0){
                    range.value().end = std::stoi(matches[2]);
                    if (range->end < range->start){
                        throw std::invalid_argument("Range end can't be smaller than range start");
                    }
                }
            }
        }
    }
}


HttpRequest::Method HttpRequest::get_method() const {
    return method;
}

std::string HttpRequest::get_version() const {
    return http_version;
}

std::string HttpRequest::get_uri() const {
    return uri;
}

const std::optional<Range> & HttpRequest::get_range() const {
    return range;
}

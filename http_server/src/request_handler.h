//
// Created by maik2 on 12.12.2021.
//

#ifndef RN_REQUEST_HANDLER_H
#define RN_REQUEST_HANDLER_H

#include <string>
#include <vector>

class HttpRequest {
public:
    explicit HttpRequest(const std::string &data);
    enum Method { GET, POST, PUT };

    [[maybe_unused]] Method get_method();
    std::string get_version();
    std::string get_uri();
    std::string get_headers();

private:
    Method method = GET;
    std::string uri;
    std::string http_version;
    std::string headers;
};

#endif //RN_REQUEST_HANDLER_H

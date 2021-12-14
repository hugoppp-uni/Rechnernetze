//
// Created by maik2 on 14.12.2021.
//

#ifndef RN_HTTPREQUEST_H
#define RN_HTTPREQUEST_H

#include <string>

class HttpRequest {
public:
    explicit HttpRequest(const std::string &data);
    enum Method { GET, POST, PUT };

    Method get_method();
    std::string get_version();
    std::string get_uri();
    std::string get_headers();

private:
    Method method = GET;
    std::string uri;
    std::string http_version;
    std::string headers;
};

#endif //RN_HTTPREQUEST_H

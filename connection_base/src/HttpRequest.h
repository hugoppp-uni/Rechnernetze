#pragma once

#include <map>
#include <string>

class HttpRequest {
public:
    explicit HttpRequest(const std::string &data);
    enum class Method { GET, POST, PUT };

    Method get_method() const;
    std::string get_version() const;
    std::string get_uri() const;

private:
    Method method = Method::GET;
    std::string uri {};
    std::string http_version {};
    std::map<std::string, std::string> headers {};
    std::optional<std::tuple<int,int>> range{std::nullopt};
};


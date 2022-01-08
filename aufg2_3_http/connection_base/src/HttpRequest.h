#pragma once

#include <map>
#include <string>

struct Range{
    int start;
    std::optional<int> end;
};

class HttpRequest {
public:
    explicit HttpRequest(const std::string &data);
    enum class Method { NONE, GET, POST, PUT };

    [[nodiscard]] Method get_method() const;
    [[nodiscard]] std::string get_version() const;
    [[nodiscard]] std::string get_uri() const;
    [[nodiscard]] const std::optional<Range> &get_range() const;

private:
    Method method = Method::GET;
    std::string uri {};
    std::string http_version {};
    std::map<std::string, std::string> headers {};
    std::optional<Range> range{std::nullopt};
};


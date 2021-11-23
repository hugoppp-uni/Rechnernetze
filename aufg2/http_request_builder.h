#pragma once

#include <string>
#include <vector>
#include <sstream>
#include "url_info.hpp"

class HttpRequestBuilder {
public:
    HttpRequestBuilder(const UrlInfo& url_info);

    void add(const std::string& key, const std::string& value);
    void add_range(const std::string& value);
    std::string to_string();
private:
    std::stringstream result_stream;


};




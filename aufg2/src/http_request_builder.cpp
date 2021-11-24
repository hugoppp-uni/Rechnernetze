#include <regex>
#include "http_request_builder.h"

HttpRequestBuilder::HttpRequestBuilder(const UrlInfo& url_info) {
    result_stream << "GET /"<< url_info.file << " HTTP/1.1\r\n";
    add("Host", url_info.host);
}

void HttpRequestBuilder::add(const std::string &key, const std::string &value) {
    result_stream << key << ": " << value << "\r\n";
}

std::string HttpRequestBuilder::to_string() {
    return result_stream.str() + "\r\n\r\n";
}

void HttpRequestBuilder::add_range(const std::string &value) {
    add("Range", "bytes=" + value);
}

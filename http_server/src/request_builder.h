#pragma once

#include <HttpRequest.h>
#include <memory>
#include <connection.hpp>


class ResponseBuilder {
public:
    static std::string build(const HttpRequest &request, const std::string &document_root);

private:
};

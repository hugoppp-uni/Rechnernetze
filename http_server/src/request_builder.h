#pragma once

#include <HttpRequest.h>
#include <memory>
#include <connection.hpp>

namespace response_builder {

std::string build(const HttpRequest &request, const std::string &document_root);

}
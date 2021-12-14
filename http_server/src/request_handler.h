//
// Created by maik2 on 12.12.2021.
//

#ifndef RN_REQUEST_HANDLER_H
#define RN_REQUEST_HANDLER_H

#include <HttpRequest.h>
#include <memory>
#include <connection.hpp>

int handle_request(const std::unique_ptr<Connection>& cnn, HttpRequest *request);

#endif //RN_REQUEST_HANDLER_H

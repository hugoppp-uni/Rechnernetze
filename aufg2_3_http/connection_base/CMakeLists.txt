cmake_minimum_required(VERSION 3.16)
project(connection_base)

set(CMAKE_CXX_STANDARD 20)

add_library(connection_base STATIC
        src/connection.cpp
        src/connection.hpp
        src/HttpResponse.hpp
        src/HttpResponse.cpp
        src/address.cpp
        src/address.hpp
        src/logger.hpp
        src/HttpRequest.cpp
        src/HttpRequest.h)

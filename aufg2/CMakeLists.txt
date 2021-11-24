cmake_minimum_required(VERSION 3.16)
project(aufg2)

set(CMAKE_CXX_STANDARD 20)

add_executable(aufg2
        src/main.cpp
        src/connection.cpp
        src/http_request_builder.cpp
        src/http_request_builder.h
        src/url_info.cpp
        src/url_info.hpp
        src/Response.h
        src/Response.cpp
        )
target_include_directories(aufg2 PRIVATE lib/cxxopts/include)

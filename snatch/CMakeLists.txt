cmake_minimum_required(VERSION 3.16)
project(snatch)

set(CMAKE_CXX_STANDARD 20)

add_executable(snatch
        src/main.cpp
        src/connection.cpp
        src/http_request_builder.cpp
        src/http_request_builder.h
        src/url_info.cpp
        src/url_info.hpp
        src/http_response.h
        src/http_response.cpp
        )
target_include_directories(snatch PRIVATE lib/cxxopts/include)

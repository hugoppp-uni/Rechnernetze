cmake_minimum_required(VERSION 3.16)
project(http_server)

set(CMAKE_CXX_STANDARD 20)

add_executable(http_server
        src/main.cpp
        src/connection_listener.cpp
        src/connection_listener.hpp
        ../aufg2/src/connection.cpp
        ../aufg2/src/connection.hpp
        ../aufg2/src/http_response.cpp
        ../aufg2/src/http_response.h
        )

target_include_directories(http_server PRIVATE lib/cxxopts/include)

cmake_minimum_required(VERSION 3.16)
project(http_server)

set(CMAKE_CXX_STANDARD 20)

add_executable(http_server
        src/main.cpp
        src/connection_listener.cpp
        src/connection_listener.hpp
        ../snatch/src/connection.cpp
        ../snatch/src/connection.hpp
        ../snatch/src/http_response.cpp
        ../snatch/src/http_response.h
        ../snatch/src/address.cpp
        ../snatch/src/address.hpp
        )

target_include_directories(http_server PRIVATE lib/cxxopts/include)

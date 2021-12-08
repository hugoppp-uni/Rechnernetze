cmake_minimum_required(VERSION 3.16)
project(http_server)

set(CMAKE_CXX_STANDARD 20)

add_executable(http_server
src/main.cpp
src/connection_listener.cpp
src/connection_listener.hpp
src/client_connection.cpp
src/client_connection.hpp)

target_include_directories(http_server PRIVATE lib/cxxopts/include)

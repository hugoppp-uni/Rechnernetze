cmake_minimum_required(VERSION 3.16)
project(http_server)

set(CMAKE_CXX_STANDARD 20)

add_executable(http_server
        src/main.cpp
        src/connection_listener.cpp
        src/connection_listener.hpp
        src/request_builder.cpp
        src/request_builder.h
        )

target_include_directories(http_server PRIVATE lib/cxxopts/include)
target_include_directories(http_server PRIVATE ${CMAKE_SOURCE_DIR}/connection_base/src)

find_package(Threads REQUIRED)
target_link_libraries(http_server PRIVATE Threads::Threads)
target_link_libraries(http_server PRIVATE connection_base)

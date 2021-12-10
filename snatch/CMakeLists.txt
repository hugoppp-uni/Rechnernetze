cmake_minimum_required(VERSION 3.16)
project(snatch)

set(CMAKE_CXX_STANDARD 20)

add_executable(snatch
        src/main.cpp
        src/http_request_builder.cpp
        src/http_request_builder.h
        src/url_info.cpp
        src/url_info.hpp
        )

target_link_libraries(snatch PRIVATE connection_base)

target_include_directories(snatch PRIVATE lib/cxxopts/include)
target_include_directories(snatch PRIVATE ${CMAKE_SOURCE_DIR}/connection_base/src)

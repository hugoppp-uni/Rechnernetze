cmake_minimum_required(VERSION 3.16)
project(http_server)

set(CMAKE_CXX_STANDARD 20)

include(FetchContent)
FetchContent_Declare(fmt
        GIT_REPOSITORY https://github.com/fmtlib/fmt.git
        GIT_TAG 8.0.1)
FetchContent_MakeAvailable(fmt)

add_executable(http_server
        src/main.cpp
        src/connection_listener.cpp
        src/connection_listener.hpp
        src/response_factory.cpp
        src/response_factory.h
        src/helper.cpp src/helper.hpp)

target_include_directories(http_server PRIVATE
        lib/include
        ${CMAKE_SOURCE_DIR}/connection_base/src
        )

find_package(Threads REQUIRED)

target_link_libraries(http_server PRIVATE
        connection_base
        Threads::Threads
        fmt::fmt
        )

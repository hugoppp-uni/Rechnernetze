cmake_minimum_required(VERSION 3.16)
project(bft_server)

set(CMAKE_CXX_STANDARD 20)

add_executable(bft_server
        main.cpp
        file_writer.cpp
        file_writer.hpp
        )

target_include_directories(bft_server PRIVATE
        ${CMAKE_SOURCE_DIR}/bft_base
        )

target_link_libraries(bft_server bft_base)

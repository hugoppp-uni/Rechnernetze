cmake_minimum_required(VERSION 3.16)
project(bft_client)

set(CMAKE_CXX_STANDARD 20)

add_executable(bft_client
        main.cpp
        options.hpp
        )

target_include_directories(bft_client PRIVATE
        ${CMAKE_SOURCE_DIR}/bft_base
        )

target_link_libraries(bft_client bft_base)

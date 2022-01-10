cmake_minimum_required(VERSION 3.16)
project(bft_base)

set(CMAKE_CXX_STANDARD 20)

add_library(bft_base STATIC
        bft_datagram.hpp
        bft_datagram.cpp
        logger.hpp
        lib/include/CRC.h
        )

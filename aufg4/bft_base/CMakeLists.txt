cmake_minimum_required(VERSION 3.16)
project(bft_base)

set(CMAKE_CXX_STANDARD 20)

#add_library(bft_base STATIC
#        bft_datagram.hpp
#        logger.hpp
#        lib/include/CRC.h
#        )

add_library(bft_base STATIC
        Timer.cpp
        Timer.hpp
        )

target_include_directories(bft_base INTERFACE
        lib/include
        )
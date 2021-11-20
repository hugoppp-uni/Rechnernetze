cmake_minimum_required(VERSION 3.20)
project(aufg2)

set(CMAKE_CXX_STANDARD 20)

add_executable(aufg2 main.cpp)
target_include_directories(aufg2 PRIVATE lib/cxxopts/include)

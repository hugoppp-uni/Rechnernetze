cmake_minimum_required(VERSION 3.16)
project(rn)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=thread -fPIE -pie -g")
#[-OX]
add_subdirectory(snatch)
add_subdirectory(http_server)
add_subdirectory(connection_base)

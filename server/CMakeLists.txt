cmake_minimum_required(VERSION 3.19)
project(Server)

set(CMAKE_CXX_STANDARD 17)

include_directories(.)
include_directories(/home/karma/CLionProjects/json-develop/include)

add_executable(main main.cpp
        Server.h
        Server.cpp
        )

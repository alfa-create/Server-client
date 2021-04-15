cmake_minimum_required(VERSION 3.19)
project(Client)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)
include_directories(/home/karma/CLionProjects/json-develop/include)


add_executable(Client
        main.cpp)

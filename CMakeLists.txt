cmake_minimum_required(VERSION 3.29)

set(CMAKE_C_COMPILER gcc)
set(CMAKE_CXX_COMPILER g++)
set(CMAKE_GENERATOR "MinGW Makefiles")

project(start_up)

set(CMAKE_CXX_STANDARD 17)

add_executable(start_up main.cpp)

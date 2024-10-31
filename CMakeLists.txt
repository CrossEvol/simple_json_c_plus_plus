cmake_minimum_required(VERSION 3.29)

set(CMAKE_C_COMPILER gcc)
set(CMAKE_CXX_COMPILER g++)
set(CMAKE_GENERATOR "MinGW Makefiles")

project(start_up)

set(CMAKE_CXX_STANDARD 17)

# Include FetchContent
include(FetchContent)

# Declare fmt
FetchContent_Declare(
    fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt.git
    GIT_TAG 10.2.1
)

# Declare Catch2
FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.5.2
)

# Make the libraries available
FetchContent_MakeAvailable(fmt Catch2)

# Add sub directories
add_subdirectory(simple_json)
add_subdirectory(example)


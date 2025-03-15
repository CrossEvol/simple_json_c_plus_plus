cmake_minimum_required(VERSION 3.29)

# Set C++ compiler to GCC/G++ if not specified
if(NOT CMAKE_CXX_COMPILER)
    set(CMAKE_CXX_COMPILER g++)
endif()
if(NOT CMAKE_C_COMPILER)
    set(CMAKE_C_COMPILER gcc)
endif()

# Set the project name
project(simple_json)

# Set C++20 as the standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

############################################################
# Fetch dependencies
############################################################

include(FetchContent)

# Fetch Catch2 for testing
FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v3.4.0  # Use a specific version tag for stability
)
FetchContent_MakeAvailable(Catch2)

############################################################
# Create a library
############################################################

add_library(${PROJECT_NAME}
        STATIC
        src/simple_json.cpp
)

target_include_directories(${PROJECT_NAME}
        PUBLIC
        ${PROJECT_SOURCE_DIR}/src/include
)

FetchContent_Declare(
        fmt
        GIT_REPOSITORY https://github.com/fmtlib/fmt.git
        GIT_TAG 9.1.0
)
FetchContent_MakeAvailable(fmt)

# Remove this line if you don't need fmt
target_link_directories(${PROJECT_NAME} PRIVATE fmt::fmt)

############################################################
# Create Test executable
############################################################

set(TEST_SOURCES
        ${PROJECT_SOURCE_DIR}/test/parse_literal_test.cpp
        ${PROJECT_SOURCE_DIR}/test/parse_number_test.cpp
        ${PROJECT_SOURCE_DIR}/test/parse_string_test.cpp
        ${PROJECT_SOURCE_DIR}/test/parse_array_test.cpp
        ${PROJECT_SOURCE_DIR}/test/parse_object_test.cpp
        ${PROJECT_SOURCE_DIR}/test/parse_exception_test.cpp
)

add_executable(simple_json_tests ${TEST_SOURCES})

# Link libraries to test executable
target_link_libraries(simple_json_tests PRIVATE
        simple_json
        Catch2::Catch2WithMain
)
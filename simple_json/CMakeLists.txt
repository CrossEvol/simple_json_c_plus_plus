# Set the project name
project(simple_json)

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

target_link_directories(${PROJECT_NAME} PRIVATE fmt::fmt)


############################################################
# Create Test executable
############################################################

set(TEST_SOURCES
        ${PROJECT_SOURCE_DIR}/test/simple_json_tests.cpp
        ${PROJECT_SOURCE_DIR}/test/parse_literal_test.cpp
        ${PROJECT_SOURCE_DIR}/test/parse_number_test.cpp
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
# Set the project name
project(simple_json)

############################################################
# Create a library
############################################################

add_library(hello_library STATIC
        "${PROJECT_SOURCE_DIR}/src/hello.cpp"
)
add_library(hello::library ALIAS hello_library)

target_include_directories(hello_library
        PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(hello_library PRIVATE fmt::fmt)

############################################################
# Create a library
############################################################
add_library(${PROJECT_NAME} src/simple_json.cpp)
target_link_directories(${PROJECT_NAME} PRIVATE hello::library)


############################################################
# Create Test executable
############################################################

add_executable(simple_json_tests
        ${PROJECT_SOURCE_DIR}/test/simple_json_tests.cpp
)

# Link libraries to test executable
target_link_libraries(simple_json_tests PRIVATE
        hello::library
        Catch2::Catch2WithMain
)
cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
project(kari)

file(GLOB test_sources "*.cpp" "*.hpp")
add_executable(${PROJECT_NAME} ${test_sources})

set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO)

enable_testing()
add_test(${PROJECT_NAME} ${PROJECT_NAME})

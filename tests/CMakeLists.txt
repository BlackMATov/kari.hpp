cmake_minimum_required(VERSION 3.5)
project(kari)
file(GLOB kari_src "*.cpp" "*.hpp")
add_executable(${PROJECT_NAME} ${kari_src})
set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO)

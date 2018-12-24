cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
project(kari)

#
# coverage mode
#

option(KARI_BUILD_WITH_COVERAGE "Build with coverage" OFF)
if(KARI_BUILD_WITH_COVERAGE AND (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang"))
    add_definitions(-DKARI_BUILD_WITH_COVERAGE)
    set(KARI_COVERAGE_FLAGS "--coverage")
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} ${KARI_COVERAGE_FLAGS}")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${KARI_COVERAGE_FLAGS}")
    set(CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} ${KARI_COVERAGE_FLAGS}")
endif()

#
# sanitizer mode
#

option(KARI_BUILD_WITH_SANITIZER "Build with sanitizer" OFF)
if(KARI_BUILD_WITH_SANITIZER AND (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang"))
    add_definitions(-DKARI_BUILD_WITH_SANITIZER)
    set(KARI_SANITIZER_FLAGS "-fno-omit-frame-pointer -fsanitize=address")
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} ${KARI_SANITIZER_FLAGS}")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${KARI_SANITIZER_FLAGS}")
    set(CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} ${KARI_SANITIZER_FLAGS}")
endif()

#
# tests executable
#

file(GLOB test_sources "*.cpp" "*.hpp")
add_executable(${PROJECT_NAME} ${test_sources})

set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO)

enable_testing()
add_test(${PROJECT_NAME} ${PROJECT_NAME})

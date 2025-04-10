project(kari.hpp.untests)

file(GLOB_RECURSE UNTESTS_SOURCES CONFIGURE_DEPENDS "*.cpp" "*.hpp")
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${UNTESTS_SOURCES})

add_executable(${PROJECT_NAME} ${UNTESTS_SOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE
    kari.hpp::kari.hpp
    kari.hpp.vendors::doctest)

target_compile_options(${PROJECT_NAME} PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:
        /WX /W4 /bigobj>
    $<$<CXX_COMPILER_ID:GNU>:
        -Werror -Wall -Wextra -Wpedantic
        -Wno-inaccessible-base>
    $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>>:
        -Werror -Weverything -Wconversion
        -Wno-c++98-compat
        -Wno-c++98-compat-pedantic
        -Wno-exit-time-destructors
        -Wno-global-constructors
        -Wno-padded
        -Wno-poison-system-directories
        -Wno-switch-default
        -Wno-unknown-warning-option
        -Wno-unneeded-internal-declaration
        -Wno-unsafe-buffer-usage
        -Wno-unused-macros
        -Wno-unused-member-function
        -Wno-weak-vtables
        -Wno-zero-as-null-pointer-constant
        >)

add_test(${PROJECT_NAME} ${PROJECT_NAME})

project(kari.hpp.vendors)

add_library(${PROJECT_NAME}.doctest STATIC EXCLUDE_FROM_ALL doctest/doctest/parts/doctest.cpp)
add_library(${PROJECT_NAME}::doctest ALIAS ${PROJECT_NAME}.doctest)

target_compile_features(${PROJECT_NAME}.doctest
    PUBLIC cxx_std_20)

target_include_directories(${PROJECT_NAME}.doctest SYSTEM
    PUBLIC doctest)

target_compile_definitions(${PROJECT_NAME}.doctest
    PRIVATE
        DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
    PUBLIC
        DOCTEST_CONFIG_USE_STD_HEADERS
        $<$<BOOL:${BUILD_WITH_NO_EXCEPTIONS}>:
            DOCTEST_CONFIG_NO_EXCEPTIONS_BUT_WITH_ALL_ASSERTS>)

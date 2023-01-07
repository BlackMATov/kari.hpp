cmake_minimum_required(VERSION 3.21 FATAL_ERROR)

project(kari.hpp
    VERSION "0.0.1"
    DESCRIPTION "Experimental library for currying in C++17"
    HOMEPAGE_URL "https://github.com/blackmatov/kari.hpp")

#
# LIBRARY
#

add_library(${PROJECT_NAME} INTERFACE)
add_library(kari.hpp::kari.hpp ALIAS ${PROJECT_NAME})

target_compile_features(${PROJECT_NAME} INTERFACE
    cxx_std_17)

target_include_directories(${PROJECT_NAME} INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/headers>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

#
# INSTALL
#

if(PROJECT_IS_TOP_LEVEL)
    include(CMakePackageConfigHelpers)
    include(GNUInstallDirs)

    set(KARI_HPP_INSTALL_CONFIG_DIR
        "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}")

    set(KARI_HPP_INSTALL_CONFIG_INPUT
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Config.cmake.in")

    set(KARI_HPP_INSTALL_GENERATED_CONFIG_CMAKE
        "${CMAKE_CURRENT_BINARY_DIR}/generated/${PROJECT_NAME}-config.cmake")

    set(KARI_HPP_INSTALL_GENERATED_CONFIG_VERSION_CMAKE
        "${CMAKE_CURRENT_BINARY_DIR}/generated/${PROJECT_NAME}-config-version.cmake")

    configure_package_config_file(
        "${KARI_HPP_INSTALL_CONFIG_INPUT}"
        "${KARI_HPP_INSTALL_GENERATED_CONFIG_CMAKE}"
        INSTALL_DESTINATION "${KARI_HPP_INSTALL_CONFIG_DIR}"
        NO_SET_AND_CHECK_MACRO
        NO_CHECK_REQUIRED_COMPONENTS_MACRO)

    write_basic_package_version_file(
        "${KARI_HPP_INSTALL_GENERATED_CONFIG_VERSION_CMAKE}"
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY AnyNewerVersion
        ARCH_INDEPENDENT)

    install(
        TARGETS ${PROJECT_NAME}
        EXPORT ${PROJECT_NAME}-targets
        INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")

    install(
        DIRECTORY headers/${PROJECT_NAME}
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")

    install(
        EXPORT ${PROJECT_NAME}-targets
        FILE ${PROJECT_NAME}-targets.cmake
        NAMESPACE ${PROJECT_NAME}::
        DESTINATION "${KARI_HPP_INSTALL_CONFIG_DIR}")

    install(
        FILES "${KARI_HPP_INSTALL_GENERATED_CONFIG_CMAKE}"
              "${KARI_HPP_INSTALL_GENERATED_CONFIG_VERSION_CMAKE}"
        DESTINATION "${KARI_HPP_INSTALL_CONFIG_DIR}")
endif()

#
# DEVELOPER
#

if(PROJECT_IS_TOP_LEVEL)
    option(BUILD_WITH_COVERAGE "Build with coverage" OFF)
    option(BUILD_WITH_SANITIZERS "Build with sanitizers" OFF)

    enable_testing()
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)
    set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMake")
    list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

    include(EnableASan)
    include(EnableGCov)
    include(EnableUBSan)

    add_subdirectory(vendors)
    add_subdirectory(untests)
endif()

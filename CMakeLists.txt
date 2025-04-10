cmake_minimum_required(VERSION 3.21 FATAL_ERROR)

project(kari.hpp
    VERSION "0.0.1"
    DESCRIPTION "Experimental library for currying in C++17"
    HOMEPAGE_URL "https://github.com/blackmatov/kari.hpp"
    LANGUAGES CXX)

option(KARI_HPP_DEVELOP "Generate develop targets" OFF)
option(KARI_HPP_INSTALL "Generate install targets" ${PROJECT_IS_TOP_LEVEL})

add_library(${PROJECT_NAME} INTERFACE)
add_library(kari.hpp::kari.hpp ALIAS ${PROJECT_NAME})

target_compile_features(${PROJECT_NAME} INTERFACE
    cxx_std_17)

target_include_directories(${PROJECT_NAME} INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/headers>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

if(KARI_HPP_DEVELOP)
    enable_testing()
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)
    set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMake")
    list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

    add_subdirectory(vendors)
    add_subdirectory(untests)
endif()

if(KARI_HPP_INSTALL)
    include(CMakePackageConfigHelpers)
    include(GNUInstallDirs)

    set(KARI_HPP_INSTALL_CONFIG_DIR
        "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}")

    set(KARI_HPP_INSTALL_CONFIG_INPUT
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/config.cmake.in")

    set(KARI_HPP_INSTALL_CONFIG_CMAKE
        "${CMAKE_CURRENT_BINARY_DIR}/configs/${PROJECT_NAME}-config.cmake")

    set(KARI_HPP_INSTALL_CONFIG_VERSION_CMAKE
        "${CMAKE_CURRENT_BINARY_DIR}/configs/${PROJECT_NAME}-config-version.cmake")

    configure_package_config_file(
        "${KARI_HPP_INSTALL_CONFIG_INPUT}"
        "${KARI_HPP_INSTALL_CONFIG_CMAKE}"
        INSTALL_DESTINATION "${KARI_HPP_INSTALL_CONFIG_DIR}"
        NO_SET_AND_CHECK_MACRO)

    write_basic_package_version_file(
        "${KARI_HPP_INSTALL_CONFIG_VERSION_CMAKE}"
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY AnyNewerVersion
        ARCH_INDEPENDENT)

    install(
        TARGETS ${PROJECT_NAME}
        EXPORT ${PROJECT_NAME}-targets)

    install(
        DIRECTORY headers/${PROJECT_NAME}
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")

    install(
        EXPORT ${PROJECT_NAME}-targets
        FILE ${PROJECT_NAME}-targets.cmake
        NAMESPACE ${PROJECT_NAME}::
        DESTINATION "${KARI_HPP_INSTALL_CONFIG_DIR}")

    install(
        FILES "${KARI_HPP_INSTALL_CONFIG_CMAKE}"
              "${KARI_HPP_INSTALL_CONFIG_VERSION_CMAKE}"
        DESTINATION "${KARI_HPP_INSTALL_CONFIG_DIR}")
endif()

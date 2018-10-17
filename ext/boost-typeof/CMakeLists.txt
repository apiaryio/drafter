# Copyright 2018 Thomas Jandecka
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt

cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

project(BoostTypeof VERSION 1.66 LANGUAGES CXX)

add_library(typeof INTERFACE)

target_include_directories(typeof INTERFACE 
    $<BUILD_INTERFACE:${BoostTypeof_BINARY_DIR}/include>
    $<BUILD_INTERFACE:${BoostTypeof_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    )

find_package(BoostConfig 1.66 REQUIRED)
find_package(BoostMpl 1.66 REQUIRED)
find_package(BoostPreprocessor 1.66 REQUIRED)
find_package(BoostTypeTraits 1.66 REQUIRED)
find_package(BoostUtility 1.66 REQUIRED)

target_link_libraries(typeof
    INTERFACE
        Boost::config
        Boost::mpl
        Boost::preprocessor
        Boost::type_traits
        Boost::utility
    )

install(EXPORT typeof-targets
    FILE BoostTypeofTargets.cmake
    NAMESPACE Boost::
    DESTINATION lib/cmake/boost
    )

install(TARGETS typeof EXPORT typeof-targets
    INCLUDES DESTINATION include
    )

include(CMakePackageConfigHelpers)
write_basic_package_version_file("BoostTypeofConfigVersion.cmake"
    VERSION ${BoostTypeof_VERSION}
    COMPATIBILITY SameMajorVersion
    )
install(
    FILES
        "BoostTypeofConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/BoostTypeofConfigVersion.cmake"
    DESTINATION
        lib/cmake/boost
    )
install(DIRECTORY include/ DESTINATION include)

add_library(Boost::typeof ALIAS typeof)

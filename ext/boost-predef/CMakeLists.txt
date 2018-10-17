# Copyright 2018 Thomas Jandecka
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt

cmake_minimum_required(VERSION 3.4 FATAL_ERROR)

project(BoostPredef VERSION 1.66 LANGUAGES CXX)

add_library(predef INTERFACE)

target_include_directories(predef INTERFACE 
    $<BUILD_INTERFACE:${BoostPredef_BINARY_DIR}/include>
    $<BUILD_INTERFACE:${BoostPredef_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    )

install(EXPORT predef-targets
    FILE BoostPredefTargets.cmake
    NAMESPACE Boost::
    DESTINATION lib/cmake/boost
    )

install(TARGETS predef EXPORT predef-targets
    INCLUDES DESTINATION include
    )

include(CMakePackageConfigHelpers)
write_basic_package_version_file("BoostPredefConfigVersion.cmake"
    VERSION ${BoostPredef_VERSION}
    COMPATIBILITY SameMajorVersion
    )
install(
    FILES
        "BoostPredefConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/BoostPredefConfigVersion.cmake"
    DESTINATION
        lib/cmake/boost
    )
install(DIRECTORY include/ DESTINATION include)

add_library(Boost::predef ALIAS predef)

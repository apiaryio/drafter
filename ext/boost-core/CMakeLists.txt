# Copyright 2018 Thomas Jandecka
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt

cmake_minimum_required(VERSION 3.4 FATAL_ERROR)

project(BoostCore VERSION 1.66 LANGUAGES CXX)

add_library(core INTERFACE)

target_include_directories(core INTERFACE 
    $<BUILD_INTERFACE:${BoostCore_BINARY_DIR}/include>
    $<BUILD_INTERFACE:${BoostCore_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    )

find_package(BoostAssert 1.66 REQUIRED)
find_package(BoostConfig 1.66 REQUIRED)

target_link_libraries(core
    INTERFACE
        Boost::assert
        Boost::config
    )

install(EXPORT core-targets
    FILE BoostCoreTargets.cmake
    NAMESPACE Boost::
    DESTINATION lib/cmake/boost
    )

install(TARGETS core EXPORT core-targets
    INCLUDES DESTINATION include
    )

include(CMakePackageConfigHelpers)
write_basic_package_version_file("BoostCoreConfigVersion.cmake"
    VERSION ${BoostCore_VERSION}
    COMPATIBILITY SameMajorVersion
    )
install(
    FILES
        "BoostCoreConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/BoostCoreConfigVersion.cmake"
    DESTINATION
        lib/cmake/boost
    )
install(DIRECTORY include/ DESTINATION include)

add_library(Boost::core ALIAS core)

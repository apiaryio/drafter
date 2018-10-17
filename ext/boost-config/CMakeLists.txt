# Copyright 2018 Thomas Jandecka
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt

cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

project(BoostConfig VERSION 1.66 LANGUAGES CXX)

add_library(config INTERFACE)

target_include_directories(config INTERFACE 
    $<BUILD_INTERFACE:${BoostConfig_BINARY_DIR}/include>
    $<BUILD_INTERFACE:${BoostConfig_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    )

target_link_libraries(config INTERFACE)

install(EXPORT config-targets
    FILE BoostConfigTargets.cmake
    NAMESPACE Boost::
    DESTINATION lib/cmake/boost
    )

install(TARGETS config EXPORT config-targets
    INCLUDES DESTINATION include
    )

include(CMakePackageConfigHelpers)
write_basic_package_version_file("BoostConfigConfigVersion.cmake"
    VERSION ${BoostConfig_VERSION}
    COMPATIBILITY SameMajorVersion
    )
install(
    FILES
        "BoostConfigConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/BoostConfigConfigVersion.cmake"
    DESTINATION
        lib/cmake/boost
    )
install(DIRECTORY include/ DESTINATION include)

add_library(Boost::config ALIAS config)

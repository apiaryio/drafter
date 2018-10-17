# Copyright 2018 Thomas Jandecka
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt

cmake_minimum_required(VERSION 3.4 FATAL_ERROR)

project(BoostInteger VERSION 1.66 LANGUAGES CXX)

add_library(integer INTERFACE)

target_include_directories(integer INTERFACE 
    $<BUILD_INTERFACE:${BoostInteger_BINARY_DIR}/include>
    $<BUILD_INTERFACE:${BoostInteger_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    )

find_package(BoostAssert 1.66 REQUIRED)
find_package(BoostConfig 1.66 REQUIRED)
find_package(BoostCore 1.66 REQUIRED)
find_package(BoostStaticAssert 1.66 REQUIRED)

target_link_libraries(integer
    INTERFACE
        Boost::assert
        Boost::config
        Boost::core
        Boost::static_assert
    )

install(EXPORT integer-targets
    FILE BoostIntegerTargets.cmake
    NAMESPACE Boost::
    DESTINATION lib/cmake/boost
    )

install(TARGETS integer EXPORT integer-targets
    INCLUDES DESTINATION include
    )

include(CMakePackageConfigHelpers)
write_basic_package_version_file("BoostIntegerConfigVersion.cmake"
    VERSION ${BoostInteger_VERSION}
    COMPATIBILITY SameMajorVersion
    )
install(
    FILES
        "BoostIntegerConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/BoostIntegerConfigVersion.cmake"
    DESTINATION
        lib/cmake/boost
    )
install(DIRECTORY include/ DESTINATION include)

add_library(Boost::integer ALIAS integer)

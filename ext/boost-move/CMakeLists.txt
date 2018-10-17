# Copyright 2018 Thomas Jandecka
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt

cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

project(BoostMove VERSION 1.66 LANGUAGES CXX)

add_library(move INTERFACE)

target_include_directories(move INTERFACE 
    $<BUILD_INTERFACE:${BoostMove_BINARY_DIR}/include>
    $<BUILD_INTERFACE:${BoostMove_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    )

find_package(BoostCore 1.66 REQUIRED)
find_package(BoostStaticAssert 1.66 REQUIRED)

target_link_libraries(move
    INTERFACE
        Boost::core
        Boost::static_assert
    )

install(EXPORT move-targets
    FILE BoostMoveTargets.cmake
    NAMESPACE Boost::
    DESTINATION lib/cmake/boost
    )

install(TARGETS move EXPORT move-targets
    INCLUDES DESTINATION include
    )

include(CMakePackageConfigHelpers)
write_basic_package_version_file("BoostMoveConfigVersion.cmake"
    VERSION ${BoostMove_VERSION}
    COMPATIBILITY SameMajorVersion
    )
install(
    FILES
        "BoostMoveConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/BoostMoveConfigVersion.cmake"
    DESTINATION
        lib/cmake/boost
    )
install(DIRECTORY include/ DESTINATION include)

add_library(Boost::move ALIAS move)

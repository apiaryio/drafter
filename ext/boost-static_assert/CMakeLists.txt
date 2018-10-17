# Copyright 2018 Thomas Jandecka
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt

cmake_minimum_required(VERSION 3.4 FATAL_ERROR)

project(BoostStaticAssert VERSION 1.66 LANGUAGES CXX)

add_library(static_assert INTERFACE)

target_include_directories(static_assert INTERFACE 
    $<BUILD_INTERFACE:${BoostStaticAssert_BINARY_DIR}/include>
    $<BUILD_INTERFACE:${BoostStaticAssert_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    )

find_package(BoostConfig 1.66 REQUIRED)

target_link_libraries(static_assert
    INTERFACE
        Boost::config
    )

install(EXPORT static_assert-targets
    FILE BoostStaticAssertTargets.cmake
    NAMESPACE Boost::
    DESTINATION lib/cmake/boost
    )

install(TARGETS static_assert EXPORT static_assert-targets
    INCLUDES DESTINATION include
    )

include(CMakePackageConfigHelpers)
write_basic_package_version_file("BoostStaticAssertConfigVersion.cmake"
    VERSION ${BoostStaticAssert_VERSION}
    COMPATIBILITY SameMajorVersion
    )
install(
    FILES
        "BoostStaticAssertConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/BoostStaticAssertConfigVersion.cmake"
    DESTINATION
        lib/cmake/boost
    )
install(DIRECTORY include/ DESTINATION include)

add_library(Boost::static_assert ALIAS static_assert)

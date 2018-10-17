# Copyright 2018 Thomas Jandecka
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt

cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

project(BoostAssert VERSION 1.66 LANGUAGES CXX)

set(BoostAssert_PUBLIC_HEADERS
    include/boost/assert.hpp
    include/boost/current_function.hpp
    )

add_library(assert INTERFACE)

target_include_directories(assert INTERFACE 
    $<BUILD_INTERFACE:${BoostAssert_BINARY_DIR}/include>
    $<BUILD_INTERFACE:${BoostAssert_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    )

find_package(BoostConfig 1.66 REQUIRED)

target_link_libraries(assert
    INTERFACE
        Boost::config
    )

install(EXPORT assert-targets
    FILE BoostAssertTargets.cmake
    NAMESPACE Boost::
    DESTINATION lib/cmake/boost
    )

install(TARGETS assert EXPORT assert-targets
    INCLUDES DESTINATION include
    PUBLIC_HEADER DESTINATION include/boost
    )

include(CMakePackageConfigHelpers)
write_basic_package_version_file("BoostAssertConfigVersion.cmake"
    VERSION ${BoostAssert_VERSION}
    COMPATIBILITY SameMajorVersion
    )
install(
    FILES
        "BoostAssertConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/BoostAssertConfigVersion.cmake"
    DESTINATION
        lib/cmake/boost
    )
install(FILES ${BoostAssert_PUBLIC_HEADERS} DESTINATION include/boost)

add_library(Boost::assert ALIAS assert)

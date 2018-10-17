# Copyright 2018 Thomas Jandecka
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt

cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

project(BoostThrowException VERSION 1.66 LANGUAGES CXX)

add_library(throw_exception INTERFACE)

target_include_directories(throw_exception INTERFACE 
    $<BUILD_INTERFACE:${BoostThrowException_BINARY_DIR}/include>
    $<BUILD_INTERFACE:${BoostThrowException_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    )

find_package(BoostConfig 1.66 REQUIRED)

target_link_libraries(throw_exception
    INTERFACE
        Boost::config
    )

install(EXPORT throw_exception-targets
    FILE BoostThrowExceptionTargets.cmake
    NAMESPACE Boost::
    DESTINATION lib/cmake/boost
    )

install(TARGETS throw_exception EXPORT throw_exception-targets
    INCLUDES DESTINATION include
    )

include(CMakePackageConfigHelpers)
write_basic_package_version_file("BoostThrowExceptionConfigVersion.cmake"
    VERSION ${BoostThrowException_VERSION}
    COMPATIBILITY SameMajorVersion
    )
install(
    FILES
        "BoostThrowExceptionConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/BoostThrowExceptionConfigVersion.cmake"
    DESTINATION
        lib/cmake/boost
    )
install(DIRECTORY include/ DESTINATION include)

add_library(Boost::throw_exception ALIAS throw_exception)

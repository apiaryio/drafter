# Copyright 2018 Thomas Jandecka
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt

cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

project(BoostPreprocessor VERSION 1.66 LANGUAGES CXX)

add_library(preprocessor INTERFACE)

target_include_directories(preprocessor INTERFACE 
    $<BUILD_INTERFACE:${BoostPreprocessor_BINARY_DIR}/include>
    $<BUILD_INTERFACE:${BoostPreprocessor_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    )

install(EXPORT preprocessor-targets
    FILE BoostPreprocessorTargets.cmake
    NAMESPACE Boost::
    DESTINATION lib/cmake/boost
    )

install(TARGETS preprocessor EXPORT preprocessor-targets
    INCLUDES DESTINATION include
    )

include(CMakePackageConfigHelpers)
write_basic_package_version_file("BoostPreprocessorConfigVersion.cmake"
    VERSION ${BoostPreprocessor_VERSION}
    COMPATIBILITY SameMajorVersion
    )
install(
    FILES
        "BoostPreprocessorConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/BoostPreprocessorConfigVersion.cmake"
    DESTINATION
        lib/cmake/boost
    )
install(DIRECTORY include/ DESTINATION include)

add_library(Boost::preprocessor ALIAS preprocessor)

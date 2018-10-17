# Copyright 2018 Thomas Jandecka
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt

cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

project(BoostUtility VERSION 1.66 LANGUAGES CXX)

add_library(utility INTERFACE)

target_include_directories(utility INTERFACE 
    $<BUILD_INTERFACE:${BoostUtility_BINARY_DIR}/include>
    $<BUILD_INTERFACE:${BoostUtility_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    )

find_package(BoostConfig 1.66 REQUIRED)
find_package(BoostCore 1.66 REQUIRED)
find_package(BoostStaticAssert 1.66 REQUIRED)
find_package(BoostMpl 1.66 REQUIRED)
find_package(BoostPreprocessor 1.66 REQUIRED)
find_package(BoostTypeTraits 1.66 REQUIRED)
find_package(BoostThrowException 1.66 REQUIRED)

target_link_libraries(utility
    INTERFACE
        Boost::config
        Boost::core
        Boost::static_assert
        Boost::mpl
        Boost::preprocessor
        Boost::type_traits
        Boost::throw_exception
    )

install(EXPORT utility-targets
    FILE BoostUtilityTargets.cmake
    NAMESPACE Boost::
    DESTINATION lib/cmake/boost
    )

install(TARGETS utility EXPORT utility-targets
    INCLUDES DESTINATION include
    )

include(CMakePackageConfigHelpers)
write_basic_package_version_file("BoostUtilityConfigVersion.cmake"
    VERSION ${BoostUtility_VERSION}
    COMPATIBILITY SameMajorVersion
    )
install(
    FILES
        "BoostUtilityConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/BoostUtilityConfigVersion.cmake"
    DESTINATION
        lib/cmake/boost
    )
install(DIRECTORY include/ DESTINATION include)

add_library(Boost::utility ALIAS utility)

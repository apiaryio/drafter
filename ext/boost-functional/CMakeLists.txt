# Copyright 2018 Thomas Jandecka
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt

cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

project(BoostFunctional VERSION 1.66 LANGUAGES CXX)

add_library(functional INTERFACE)

target_include_directories(functional INTERFACE 
    $<BUILD_INTERFACE:${BoostFunctional_BINARY_DIR}/include>
    $<BUILD_INTERFACE:${BoostFunctional_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    )

find_package(BoostAssert 1.66 REQUIRED)
find_package(BoostConfig 1.66 REQUIRED)
find_package(BoostCore 1.66 REQUIRED)
find_package(BoostInteger 1.66 REQUIRED)
find_package(BoostStaticAssert 1.66 REQUIRED)
find_package(BoostMpl 1.66 REQUIRED)
find_package(BoostPreprocessor 1.66 REQUIRED)
find_package(BoostTypeof 1.66 REQUIRED)
find_package(BoostTypeTraits 1.66 REQUIRED)
find_package(BoostDetail 1.66 REQUIRED)
find_package(BoostUtility 1.66 REQUIRED)

target_link_libraries(functional
    INTERFACE
        Boost::assert
        Boost::config
        Boost::core
        Boost::integer
        Boost::static_assert
        Boost::mpl
        Boost::preprocessor
        Boost::type_traits
        Boost::utility
        Boost::detail
    )

install(EXPORT functional-targets
    FILE BoostFunctionalTargets.cmake
    NAMESPACE Boost::
    DESTINATION lib/cmake/boost
    )

install(TARGETS functional EXPORT functional-targets
    INCLUDES DESTINATION include
    )

include(CMakePackageConfigHelpers)
write_basic_package_version_file("BoostFunctionalConfigVersion.cmake"
    VERSION ${BoostFunctional_VERSION}
    COMPATIBILITY SameMajorVersion
    )
install(
    FILES
        "BoostFunctionalConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/BoostFunctionalConfigVersion.cmake"
    DESTINATION
        lib/cmake/boost
    )
install(DIRECTORY include/ DESTINATION include)

add_library(Boost::functional ALIAS functional)

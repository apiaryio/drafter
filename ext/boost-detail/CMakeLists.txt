# Copyright 2018 Thomas Jandecka
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt

cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

project(BoostDetail VERSION 1.66 LANGUAGES CXX)

add_library(detail INTERFACE)

target_include_directories(detail INTERFACE 
    $<BUILD_INTERFACE:${BoostDetail_BINARY_DIR}/include>
    $<BUILD_INTERFACE:${BoostDetail_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    )

find_package(BoostCore 1.66 REQUIRED)
find_package(BoostConfig 1.66 REQUIRED)
find_package(BoostAssert 1.66 REQUIRED)
find_package(BoostStaticAssert 1.66 REQUIRED)
find_package(BoostTypeTraits 1.66 REQUIRED)
find_package(BoostUtility 1.66 REQUIRED)

target_link_libraries(detail
    INTERFACE
        Boost::core
        Boost::config
        Boost::assert
        Boost::static_assert
        Boost::type_traits
        Boost::utility
    )

install(EXPORT detail-targets
    FILE BoostDetailTargets.cmake
    NAMESPACE Boost::
    DESTINATION lib/cmake/boost
    )

install(TARGETS detail EXPORT detail-targets
    INCLUDES DESTINATION include
    )

include(CMakePackageConfigHelpers)
write_basic_package_version_file("BoostDetailConfigVersion.cmake"
    VERSION ${BoostDetail_VERSION}
    COMPATIBILITY SameMajorVersion
    )
install(
    FILES
        "BoostDetailConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/BoostDetailConfigVersion.cmake"
    DESTINATION
        lib/cmake/boost
    )
install(DIRECTORY include/ DESTINATION include)

add_library(Boost::detail ALIAS detail)

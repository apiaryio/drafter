# Copyright 2018 Thomas Jandecka
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt

cmake_minimum_required(VERSION 3.4 FATAL_ERROR)

project(BoostTypeTraits VERSION 1.66 LANGUAGES CXX)

add_library(type_traits INTERFACE)

target_include_directories(type_traits INTERFACE 
    $<BUILD_INTERFACE:${BoostTypeTraits_BINARY_DIR}/include>
    $<BUILD_INTERFACE:${BoostTypeTraits_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    )

find_package(BoostConfig 1.66 REQUIRED)
find_package(BoostCore 1.66 REQUIRED)
find_package(BoostStaticAssert 1.66 REQUIRED)

target_link_libraries(type_traits
    INTERFACE
        Boost::config
        Boost::core
        Boost::static_assert
    )

install(EXPORT type_traits-targets
    FILE BoostTypeTraitsTargets.cmake
    NAMESPACE Boost::
    DESTINATION lib/cmake/boost
    )

install(TARGETS type_traits EXPORT type_traits-targets
    INCLUDES DESTINATION include
    )

include(CMakePackageConfigHelpers)
write_basic_package_version_file("BoostTypeTraitsConfigVersion.cmake"
    VERSION ${BoostTypeTraits_VERSION}
    COMPATIBILITY SameMajorVersion
    )
install(
    FILES
        "BoostTypeTraitsConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/BoostTypeTraitsConfigVersion.cmake"
    DESTINATION
        lib/cmake/boost
    )
install(DIRECTORY include/ DESTINATION include)

add_library(Boost::type_traits ALIAS type_traits)

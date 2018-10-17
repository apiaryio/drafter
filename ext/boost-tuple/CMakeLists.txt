# Copyright 2018 Thomas Jandecka
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt

cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

project(BoostTuple VERSION 1.66 LANGUAGES CXX)

add_library(tuple INTERFACE)

target_include_directories(tuple INTERFACE 
    $<BUILD_INTERFACE:${BoostTuple_BINARY_DIR}/include>
    $<BUILD_INTERFACE:${BoostTuple_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    )

find_package(BoostConfig 1.66 REQUIRED)
find_package(BoostCore 1.66 REQUIRED)
find_package(BoostStaticAssert 1.66 REQUIRED)
find_package(BoostTypeTraits 1.66 REQUIRED)
find_package(BoostUtility 1.66 REQUIRED)

target_link_libraries(tuple
    INTERFACE
        Boost::config
        Boost::core
        Boost::static_assert
        Boost::type_traits
        Boost::utility
    )

install(EXPORT tuple-targets
    FILE BoostTupleTargets.cmake
    NAMESPACE Boost::
    DESTINATION lib/cmake/boost
    )

install(TARGETS tuple EXPORT tuple-targets
    INCLUDES DESTINATION include
    )

include(CMakePackageConfigHelpers)
write_basic_package_version_file("BoostTupleConfigVersion.cmake"
    VERSION ${BoostTuple_VERSION}
    COMPATIBILITY SameMajorVersion
    )
install(
    FILES
        "BoostTupleConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/BoostTupleConfigVersion.cmake"
    DESTINATION
        lib/cmake/boost
    )
install(DIRECTORY include/ DESTINATION include)

add_library(Boost::tuple ALIAS tuple)

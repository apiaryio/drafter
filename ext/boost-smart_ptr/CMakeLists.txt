# Copyright 2018 Thomas Jandecka
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt

cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

project(BoostSmartPtr VERSION 1.66 LANGUAGES CXX)

add_library(smart_ptr INTERFACE)

target_include_directories(smart_ptr INTERFACE 
    $<BUILD_INTERFACE:${BoostSmartPtr_BINARY_DIR}/include>
    $<BUILD_INTERFACE:${BoostSmartPtr_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    )

find_package(BoostAssert 1.66 REQUIRED)
find_package(BoostConfig 1.66 REQUIRED)
find_package(BoostCore 1.66 REQUIRED)
find_package(BoostMove 1.66 REQUIRED)
find_package(BoostPredef 1.66 REQUIRED)
find_package(BoostStaticAssert 1.66 REQUIRED)
find_package(BoostThrowException 1.66 REQUIRED)
find_package(BoostTypeTraits 1.66 REQUIRED)

target_link_libraries(smart_ptr
    INTERFACE
        Boost::assert
        Boost::config
        Boost::core
        Boost::move
        Boost::predef
        Boost::static_assert
        Boost::throw_exception
        Boost::type_traits
    )

install(EXPORT smart_ptr-targets
    FILE BoostSmartPtrTargets.cmake
    NAMESPACE Boost::
    DESTINATION lib/cmake/boost
    )

install(TARGETS smart_ptr EXPORT smart_ptr-targets
    INCLUDES DESTINATION include
    )

include(CMakePackageConfigHelpers)
write_basic_package_version_file("BoostSmartPtrConfigVersion.cmake"
    VERSION ${BoostSmartPtr_VERSION}
    COMPATIBILITY SameMajorVersion
    )
install(
    FILES
        "BoostSmartPtrConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/BoostSmartPtrConfigVersion.cmake"
    DESTINATION
        lib/cmake/boost
    )
install(DIRECTORY include/ DESTINATION include)

add_library(Boost::smart_ptr ALIAS smart_ptr)

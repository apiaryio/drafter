# Copyright 2018 Thomas Jandecka
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt

cmake_minimum_required(VERSION 3.4 FATAL_ERROR)

project(BoostMpl VERSION 1.66 LANGUAGES CXX)

add_library(mpl INTERFACE)

target_include_directories(mpl INTERFACE 
    $<BUILD_INTERFACE:${BoostMpl_BINARY_DIR}/include>
    $<BUILD_INTERFACE:${BoostMpl_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    )

find_package(BoostConfig 1.66)
find_package(BoostTypeTraits 1.66)
find_package(BoostStaticAssert 1.66)
find_package(BoostPreprocessor 1.66)

target_link_libraries(mpl
    INTERFACE
        Boost::config
        Boost::preprocessor
        Boost::type_traits
        Boost::static_assert
    )

install(EXPORT mpl-targets
    FILE BoostMplTargets.cmake
    NAMESPACE Boost::
    DESTINATION lib/cmake/boost
    )

install(TARGETS mpl EXPORT mpl-targets
    INCLUDES DESTINATION include
    )

include(CMakePackageConfigHelpers)
write_basic_package_version_file("BoostMplConfigVersion.cmake"
    VERSION ${BoostMpl_VERSION}
    COMPATIBILITY SameMajorVersion
    )
install(
    FILES
        "BoostMplConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/BoostMplConfigVersion.cmake"
    DESTINATION
        lib/cmake/boost
    )
install(DIRECTORY include/ DESTINATION include)

add_library(Boost::mpl ALIAS mpl)

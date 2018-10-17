# Copyright 2018 Thomas Jandecka
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt

cmake_minimum_required(VERSION 3.4 FATAL_ERROR)

project(BoostIntrusive VERSION 1.66 LANGUAGES CXX)

add_library(intrusive INTERFACE)

# interface
target_include_directories(intrusive INTERFACE 
    $<BUILD_INTERFACE:${BoostIntrusive_BINARY_DIR}/include>
    $<BUILD_INTERFACE:${BoostIntrusive_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    )

find_package(BoostConfig 1.66 REQUIRED)
find_package(BoostAssert 1.66 REQUIRED)
find_package(BoostCore 1.66 REQUIRED)
find_package(BoostFunctional 1.66 REQUIRED)
find_package(BoostStaticAssert 1.66 REQUIRED)
find_package(BoostMove 1.66 REQUIRED)

target_link_libraries(intrusive
    INTERFACE
        Boost::config
        Boost::assert
        Boost::core
        Boost::functional
        Boost::static_assert
        Boost::move
    )

# export
install(EXPORT intrusive-targets
    FILE BoostIntrusiveTargets.cmake
    NAMESPACE Boost::
    DESTINATION lib/cmake/boost
    )

install(TARGETS intrusive EXPORT intrusive-targets
    INCLUDES DESTINATION include
    )

include(CMakePackageConfigHelpers)
write_basic_package_version_file("BoostIntrusiveConfigVersion.cmake"
    VERSION ${BoostIntrusive_VERSION}
    COMPATIBILITY SameMajorVersion
    )
install(
    FILES
        "BoostIntrusiveConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/BoostIntrusiveConfigVersion.cmake"
    DESTINATION
        lib/cmake/boost
    )
install(DIRECTORY include/ DESTINATION include)

add_library(Boost::intrusive ALIAS intrusive)

# test export
if(BUILD_TESTING)
    add_library(intrusive_test INTERFACE)

    target_include_directories(intrusive_test INTERFACE 
        $<BUILD_INTERFACE:${BoostIntrusive_BINARY_DIR}/test>
        $<BUILD_INTERFACE:${BoostIntrusive_SOURCE_DIR}/test>
        $<INSTALL_INTERFACE:test>
        )

    target_link_libraries(intrusive_test INTERFACE intrusive)

    add_library(Boost::intrusive_test ALIAS intrusive_test)
endif()

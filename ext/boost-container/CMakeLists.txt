# Copyright 2018 Thomas Jandecka
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt

cmake_minimum_required(VERSION 3.4 FATAL_ERROR)

project(BoostContainer VERSION 1.66 LANGUAGES C CXX)

set(BoostContainer_SOURCES
    src/alloc_lib.c
    src/dlmalloc.cpp
    src/global_resource.cpp
    src/monotonic_buffer_resource.cpp
    src/pool_resource.cpp
    src/synchronized_pool_resource.cpp
    src/unsynchronized_pool_resource.cpp
    )

add_library(container SHARED ${BoostContainer_SOURCES})
add_library(container-static STATIC ${BoostContainer_SOURCES})
add_library(container-pic STATIC ${BoostContainer_SOURCES})
set_property(TARGET container-pic PROPERTY POSITION_INDEPENDENT_CODE 1)

if(UNIX)
    set_target_properties(container-static PROPERTIES OUTPUT_NAME boost_container)
else()
    set_target_properties(container-static PROPERTIES OUTPUT_NAME boost_container-static)
endif()

set_target_properties(container PROPERTIES OUTPUT_NAME boost_container)
set_target_properties(container-pic PROPERTIES OUTPUT_NAME boost_container-pic)

target_include_directories(container PUBLIC 
    $<BUILD_INTERFACE:${BoostContainer_BINARY_DIR}/include>
    $<BUILD_INTERFACE:${BoostContainer_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    )
target_include_directories(container-static PUBLIC 
    $<BUILD_INTERFACE:${BoostContainer_BINARY_DIR}/include>
    $<BUILD_INTERFACE:${BoostContainer_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    )
target_include_directories(container-pic PUBLIC 
    $<BUILD_INTERFACE:${BoostContainer_BINARY_DIR}/include>
    $<BUILD_INTERFACE:${BoostContainer_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    )

find_package(BoostAssert 1.66 REQUIRED)
find_package(BoostConfig 1.66 REQUIRED)
find_package(BoostCore 1.66 REQUIRED)
find_package(BoostFunctional 1.66 REQUIRED)
find_package(BoostIntrusive 1.66 REQUIRED)
find_package(BoostMove 1.66 REQUIRED)
find_package(BoostStaticAssert 1.66 REQUIRED)
find_package(BoostTypeTraits 1.66 REQUIRED)

target_link_libraries(container
    PUBLIC
        Boost::config
        Boost::move
        Boost::intrusive
    PRIVATE
        Boost::assert
        Boost::core
        Boost::functional
        Boost::static_assert
        Boost::type_traits
)

target_link_libraries(container-static
    PUBLIC
        Boost::config
        Boost::move
        Boost::intrusive
    PRIVATE
        Boost::assert
        Boost::core
        Boost::functional
        Boost::static_assert
        Boost::type_traits
)

target_link_libraries(container-pic
    PUBLIC
        Boost::config
        Boost::move
        Boost::intrusive
    PRIVATE
        Boost::assert
        Boost::core
        Boost::functional
        Boost::static_assert
        Boost::type_traits
)


install(TARGETS container container-static container-pic EXPORT container-targets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
    )

install(EXPORT container-targets
    FILE BoostContainerTargets.cmake
    NAMESPACE Boost::
    DESTINATION lib/cmake/boost
    )

include(CMakePackageConfigHelpers)
write_basic_package_version_file("BoostContainerConfigVersion.cmake"
    VERSION ${BoostContainer_VERSION}
    COMPATIBILITY SameMajorVersion
    )
install(FILES
        "BoostContainerConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/BoostContainerConfigVersion.cmake"
    DESTINATION
        lib/cmake/boost
    )
install(DIRECTORY include/ DESTINATION include)

add_library(Boost::container ALIAS container)
add_library(Boost::container-static ALIAS container-static)
add_library(Boost::container-pic ALIAS container-pic)

if(BUILD_TESTING)
    include(boost-container-tests.cmake)
endif()

cmake_minimum_required(VERSION 3.4 FATAL_ERROR)

project(apib VERSION 1.0 LANGUAGES CXX)

set(apib_SOURCES
    src/MediaType.cc
    )

add_library(apib ${apib_SOURCES})
set_property(TARGET apib PROPERTY POSITION_INDEPENDENT_CODE 1)

find_package(MPark.Variant 1.4 REQUIRED)
find_package(BoostContainer 1.66 REQUIRED)

target_link_libraries(apib
    PUBLIC
        Boost::container
        mpark_variant
      )

target_include_directories(apib
    PUBLIC
        $<BUILD_INTERFACE:${apib_BINARY_DIR}/include>
        $<BUILD_INTERFACE:${apib_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        $<BUILD_INTERFACE:${apib_BINARY_DIR}/src>
        $<BUILD_INTERFACE:${apib_SOURCE_DIR}/src>
    )

install(TARGETS apib EXPORT apib-targets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
    )

install(EXPORT apib-targets
    FILE apib-targets.cmake
    NAMESPACE Apiary::
    DESTINATION lib/cmake/apib
    )

include(CMakePackageConfigHelpers)
write_basic_package_version_file("apib-config-version.cmake"
    VERSION ${apib_VERSION}
    COMPATIBILITY SameMajorVersion
    )
install(
    FILES
        "apib-config.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/apib-config-version.cmake"
    DESTINATION
        lib/cmake/apib
    )

add_library(Apiary::apib ALIAS apib)

if(${BUILD_TESTING})
    add_subdirectory(test)
endif()

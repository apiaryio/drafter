cmake_minimum_required(VERSION 3.6 FATAL_ERROR)
project(DrafterProjects)

include(DefaultBuildType.cmake)

message(STATUS "Testing dependencies ${BUILD_TESTING_DEPENDENCIES}.")

set(CMAKE_PREFIX_PATH "/prefix")
set(CMAKE_SKIP_INSTALL_ALL_DEPENDENCY TRUE)
set(as_subproject
    apib
    apib-parser
    Sundown
    cmdline
    Drafter
    Catch2
    dtl
    BoostContainer
    MPark.Variant
    pegtl)

macro(find_package)
    if(NOT "${ARGV0}" IN_LIST as_subproject)
        _find_package(${ARGV})
    endif()
endmacro()

# add external dependencies living in `ext`
option(PEGTL_BUILD_TESTS OFF)
add_subdirectory(packages/sundown EXCLUDE_FROM_ALL)
add_subdirectory(packages/cmdline EXCLUDE_FROM_ALL)
add_subdirectory(packages/boost EXCLUDE_FROM_ALL)
add_subdirectory(packages/variant EXCLUDE_FROM_ALL)
add_subdirectory(packages/PEGTL EXCLUDE_FROM_ALL)

message(STATUS "Setting up tests for production...")
add_subdirectory(packages/Catch EXCLUDE_FROM_ALL)
add_subdirectory(packages/dtl EXCLUDE_FROM_ALL)

enable_testing()
include(CTest)
include(packages/Catch/contrib/Catch.cmake)

# targets developed in this super project
add_subdirectory(packages/apib EXCLUDE_FROM_ALL)
add_subdirectory(packages/apib-parser EXCLUDE_FROM_ALL)
add_subdirectory(packages/drafter)

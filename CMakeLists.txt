cmake_minimum_required(VERSION 3.4 FATAL_ERROR)

message(STATUS "Testing dependencies ${BUILD_TESTING_DEPENDENCIES}.")

set(CMAKE_PREFIX_PATH "/prefix")
set(CMAKE_SKIP_INSTALL_ALL_DEPENDENCY TRUE)
set(as_subproject
    markdown-parser
    snowcrash
    Sundown
    cmdline
    Drafter
    Catch2
    dtl
    BoostContainer)

macro(find_package)
    if(NOT "${ARGV0}" IN_LIST as_subproject)
        _find_package(${ARGV})
    endif()
endmacro()

# add external dependencies living in `ext`
add_subdirectory(ext/snowcrash/ext/markdown-parser/ext/sundown EXCLUDE_FROM_ALL)
add_subdirectory(ext/cmdline EXCLUDE_FROM_ALL)
add_subdirectory(ext/boost_1_66_0 EXCLUDE_FROM_ALL)

message(STATUS "Setting up tests for production...")
set(BUILD_TESTING ON)
add_subdirectory(test/vendor/Catch EXCLUDE_FROM_ALL)
add_subdirectory(test/vendor/dtl EXCLUDE_FROM_ALL)
include(CTest)
include(test/vendor/Catch/contrib/Catch.cmake)
enable_testing()

# targets developed in this super project
add_subdirectory(ext/snowcrash/ext/markdown-parser EXCLUDE_FROM_ALL)
add_subdirectory(ext/snowcrash EXCLUDE_FROM_ALL)

# drafter
add_subdirectory(src)
if(${BUILD_TESTING})
    add_subdirectory(test)
endif()


add_dependencies(drafter-test markdown-parser-test snowcrash-test)


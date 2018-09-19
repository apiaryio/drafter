cmake_minimum_required(VERSION 3.4 FATAL_ERROR)

set(CMAKE_PREFIX_PATH "/prefix")
set(as_subproject
    markdown-parser
    snowcrash
    sundown-apiary
    cmdline
    Drafter
    Catch2
    dtl)

macro(find_package)
    if(NOT "${ARGV0}" IN_LIST as_subproject)
        _find_package(${ARGV})
    endif()
endmacro()

# add external dependencies living in `ext`
add_subdirectory(ext/sundown)
add_subdirectory(ext/cmdline)
add_subdirectory(ext/dtl)

add_subdirectory(ext/Catch)
include(CTest)
include(ext/Catch/contrib/Catch.cmake)
enable_testing()

# targets developed in this super project
add_subdirectory(markdown-parser)
add_subdirectory(snowcrash)
add_subdirectory(drafter)

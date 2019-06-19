cmake_minimum_required(VERSION 3.6 FATAL_ERROR)

include(DefaultBuildType.cmake)

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
    BoostContainer
    MPark.Variant)

macro(find_package)
    if(NOT "${ARGV0}" IN_LIST as_subproject)
        _find_package(${ARGV})
    endif()
endmacro()

# add external dependencies living in `ext`
add_subdirectory(ext/snowcrash/ext/markdown-parser/ext/sundown EXCLUDE_FROM_ALL)
add_subdirectory(ext/cmdline EXCLUDE_FROM_ALL)
add_subdirectory(ext/boost EXCLUDE_FROM_ALL)
add_subdirectory(ext/variant EXCLUDE_FROM_ALL)

message(STATUS "Setting up tests for production...")
add_subdirectory(test/vendor/Catch EXCLUDE_FROM_ALL)
add_subdirectory(test/vendor/dtl EXCLUDE_FROM_ALL)

enable_testing()
include(CTest)
include(test/vendor/Catch/contrib/Catch.cmake)

# targets developed in this super project
add_subdirectory(ext/snowcrash/ext/markdown-parser EXCLUDE_FROM_ALL)
add_subdirectory(ext/snowcrash EXCLUDE_FROM_ALL)

# drafter
add_subdirectory(src)
if(${BUILD_TESTING})
    option(INTEGRATION_TESTS "Run integration tests" OFF)
    add_subdirectory(test)
    if(${INTEGRATION_TESTS})
        include(integration.cmake)
    endif()
    add_custom_target(drafter-test-suite ALL)
    add_dependencies(drafter-test-suite drafter-test markdown-parser-test snowcrash-test)
endif()




cmake_minimum_required(VERSION 3.4 FATAL_ERROR)

message(STATUS "Testing dependencies ${BUILD_TESTING_DEPENDENCIES}.")

set(CMAKE_PREFIX_PATH "/prefix")
set(CMAKE_SKIP_INSTALL_ALL_DEPENDENCY TRUE)
set(as_subproject
    markdown-parser
    snowcrash
    sundown-apiary
    cmdline
    Drafter
    Catch2
    dtl
    BoostAssert
    BoostConfig
    BoostCore
    BoostContainer
    BoostMove
    BoostStaticAssert
    BoostIntrusive
    BoostFunctional
    BoostMpl
    BoostTuple
    BoostTypeTraits
    BoostUtility
    BoostInteger
    BoostPredef
    BoostPreprocessor
    BoostTypeof
    BoostDetail
    BoostThrowException
    BoostSmartPtr)

macro(find_package)
    if(NOT "${ARGV0}" IN_LIST as_subproject)
        _find_package(${ARGV})
    endif()
endmacro()

if(BUILD_TESTING_DEPENDENCIES)
    message(STATUS "Setting up tests for dependencies and production...")

    set(BUILD_TESTING ON)
    add_subdirectory(ext/Catch)
    include(CTest)
    include(ext/Catch/contrib/Catch.cmake)
    enable_testing()

    add_subdirectory(ext/boost-smart_ptr) # testing dependency
else()
    message(STATUS "Skipping tests for dependencies...")

    set(BUILD_TESTING OFF)
endif()

# add external dependencies living in `ext`
add_subdirectory(ext/sundown EXCLUDE_FROM_ALL)
add_subdirectory(ext/cmdline EXCLUDE_FROM_ALL)
add_subdirectory(ext/dtl EXCLUDE_FROM_ALL)
add_subdirectory(ext/boost-config EXCLUDE_FROM_ALL)
add_subdirectory(ext/boost-assert EXCLUDE_FROM_ALL)
add_subdirectory(ext/boost-core EXCLUDE_FROM_ALL)
add_subdirectory(ext/boost-static_assert EXCLUDE_FROM_ALL)
add_subdirectory(ext/boost-move EXCLUDE_FROM_ALL)
add_subdirectory(ext/boost-container EXCLUDE_FROM_ALL)
add_subdirectory(ext/boost-intrusive EXCLUDE_FROM_ALL)
add_subdirectory(ext/boost-functional EXCLUDE_FROM_ALL)
add_subdirectory(ext/boost-mpl EXCLUDE_FROM_ALL)
add_subdirectory(ext/boost-tuple EXCLUDE_FROM_ALL)
add_subdirectory(ext/boost-type_traits EXCLUDE_FROM_ALL)
add_subdirectory(ext/boost-utility EXCLUDE_FROM_ALL)
add_subdirectory(ext/boost-integer EXCLUDE_FROM_ALL)
add_subdirectory(ext/boost-preprocessor EXCLUDE_FROM_ALL)
add_subdirectory(ext/boost-predef EXCLUDE_FROM_ALL)
add_subdirectory(ext/boost-typeof EXCLUDE_FROM_ALL)
add_subdirectory(ext/boost-detail EXCLUDE_FROM_ALL)
add_subdirectory(ext/boost-throw_exception EXCLUDE_FROM_ALL)

if(NOT BUILD_TESTING_DEPENDENCIES)
    message(STATUS "Setting up tests for production...")
    set(BUILD_TESTING ON)
    add_subdirectory(ext/Catch)
    include(CTest)
    include(ext/Catch/contrib/Catch.cmake)
    enable_testing()
endif()

# targets developed in this super project
add_subdirectory(markdown-parser EXCLUDE_FROM_ALL)
add_subdirectory(snowcrash EXCLUDE_FROM_ALL)
add_subdirectory(drafter)

add_dependencies(drafter-test markdown-parser-test snowcrash-test)

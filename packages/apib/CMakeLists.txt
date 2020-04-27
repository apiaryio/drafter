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

target_compile_features(apib
    PUBLIC 
        cxx_alignas
        cxx_alignof
        cxx_auto_type
        cxx_constexpr
        cxx_decltype
        cxx_defaulted_functions
        cxx_defaulted_move_initializers
        cxx_delegating_constructors
        cxx_deleted_functions
        cxx_final
        cxx_lambdas
        cxx_noexcept
        cxx_nullptr
        cxx_override
        cxx_range_for
        cxx_raw_string_literals
        cxx_rvalue_references
        cxx_static_assert
        cxx_trailing_return_types
        cxx_variadic_templates
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

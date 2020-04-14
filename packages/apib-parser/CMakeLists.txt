cmake_minimum_required(VERSION 3.4 FATAL_ERROR)

project(apib-parser VERSION 1.0 LANGUAGES CXX)

set(apib_parser_SOURCES
    src/MediaTypeParser.cc
    src/markdown-parser/ByteBuffer.cc
    src/markdown-parser/MarkdownNode.cc
    src/markdown-parser/MarkdownParser.cc
    src/snowcrash/Blueprint.cc
    src/snowcrash/BlueprintSourcemap.cc
    src/snowcrash/HeadersParser.cc
    src/snowcrash/HTTP.cc
    src/snowcrash/MSON.cc
    src/snowcrash/MSONOneOfParser.cc
    src/snowcrash/MSONSourcemap.cc
    src/snowcrash/MSONTypeSectionParser.cc
    src/snowcrash/MSONValueMemberParser.cc
    src/snowcrash/Section.cc
    src/snowcrash/Signature.cc
    src/snowcrash/snowcrash.cc
    src/snowcrash/UriTemplateParser.cc
    )

if(MSVC)
    set(apib_parser_SOURCES ${apib_parser_SOURCES} src/snowcrash/win/RegexMatch.cc)
else()
    set(apib_parser_SOURCES ${apib_parser_SOURCES} src/snowcrash/posix/RegexMatch.cc)
endif()

add_library(apib-parser ${apib_parser_SOURCES})
set_property(TARGET apib-parser PROPERTY POSITION_INDEPENDENT_CODE 1)

find_package(Sundown 1.0 REQUIRED)
find_package(MPark.Variant 1.4 REQUIRED)
find_package(BoostContainer 1.66 REQUIRED)
find_package(pegtl 2.8 REQUIRED)
find_package(apib 1.0 REQUIRED)

target_link_libraries(apib-parser
    PUBLIC
        Apiary::apib
        Apiary::sundown
        Boost::container
        mpark_variant
        pegtl
    )

target_compile_features(apib-parser
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

target_include_directories(apib-parser
    PUBLIC
        $<BUILD_INTERFACE:${apib-parser_BINARY_DIR}/include>
        $<BUILD_INTERFACE:${apib-parser_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${apib-parser_BINARY_DIR}/src/snowcrash>
        $<BUILD_INTERFACE:${apib-parser_SOURCE_DIR}/src/snowcrash>
        $<INSTALL_INTERFACE:src/snowcrash>
        $<BUILD_INTERFACE:${apib-parser_BINARY_DIR}/src/markdown-parser>
        $<BUILD_INTERFACE:${apib-parser_SOURCE_DIR}/src/markdown-parser>
        $<INSTALL_INTERFACE:src/markdown-parser>
    PRIVATE
        $<BUILD_INTERFACE:${apib-parser_BINARY_DIR}/src>
        $<BUILD_INTERFACE:${apib-parser_SOURCE_DIR}/src>
    )


install(TARGETS apib-parser EXPORT apib-parser-targets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
    )

install(EXPORT apib-parser-targets
    FILE apib-parser-targets.cmake
    NAMESPACE Apiary::
    DESTINATION lib/cmake/apib-parser
    )

include(CMakePackageConfigHelpers)
write_basic_package_version_file("apib-parser-config-version.cmake"
    VERSION ${apib_parser_VERSION}
    COMPATIBILITY SameMajorVersion
    )
install(
    FILES
        "apib-parser-config.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/apib-parser-config-version.cmake"
    DESTINATION
        lib/cmake/apib-parser
    )

add_library(Apiary::apib-parser ALIAS apib-parser)

if(${BUILD_TESTING})
    add_subdirectory(test)
endif()

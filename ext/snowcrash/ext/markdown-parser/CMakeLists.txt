cmake_minimum_required(VERSION 3.4 FATAL_ERROR)

project(markdown-parser VERSION 0.1 LANGUAGES CXX)

set(MARKDOWN_PARSER_SOURCES
    src/ByteBuffer.cc
    src/MarkdownNode.cc
    src/MarkdownParser.cc
    )

set(MARKDOWN_PARSER_COMPILE_FEATURES
    cxx_alias_templates
    cxx_alignas
    cxx_alignof
    cxx_attribute_deprecated
    cxx_auto_type
    cxx_constexpr
    cxx_contextual_conversions
    cxx_decltype
    cxx_decltype_auto
    cxx_defaulted_functions
    cxx_defaulted_move_initializers
    cxx_delegating_constructors
    cxx_deleted_functions
    cxx_digit_separators
    cxx_explicit_conversions
    cxx_final
    cxx_generalized_initializers
    cxx_generic_lambdas
    cxx_inheriting_constructors
    cxx_lambdas
    cxx_lambda_init_captures
    cxx_noexcept
    cxx_nonstatic_member_init
    cxx_nullptr
    cxx_override
    cxx_range_for
    cxx_raw_string_literals
    cxx_return_type_deduction
    cxx_rvalue_references
    cxx_sizeof_member
    cxx_static_assert
    cxx_strong_enums
    cxx_thread_local
    cxx_trailing_return_types
    cxx_unicode_literals
    cxx_uniform_initialization
    cxx_unrestricted_unions
    cxx_user_literals
    cxx_variable_templates
    cxx_variadic_macros
    cxx_variadic_templates
    cxx_template_template_parameters
    )

add_library(markdown-parser SHARED ${MARKDOWN_PARSER_SOURCES})
add_library(markdown-parser-static STATIC ${MARKDOWN_PARSER_SOURCES})
add_library(markdown-parser-pic STATIC ${MARKDOWN_PARSER_SOURCES})
set_property(TARGET markdown-parser-pic PROPERTY POSITION_INDEPENDENT_CODE 1)

if(UNIX)
    set_target_properties(markdown-parser-static PROPERTIES OUTPUT_NAME markdown-parser)
endif()

find_package(Sundown 1.0 REQUIRED)
find_package(BoostContainer 1.66 REQUIRED)

target_link_libraries(markdown-parser PUBLIC Apiary::sundown-pic Boost::container)
target_link_libraries(markdown-parser-static PUBLIC Apiary::sundown-static Boost::container)
target_link_libraries(markdown-parser-pic PUBLIC Apiary::sundown-pic Boost::container)

target_include_directories(markdown-parser PUBLIC
    $<BUILD_INTERFACE:${markdown-parser_BINARY_DIR}/src>
    $<BUILD_INTERFACE:${markdown-parser_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:src>
    )

target_include_directories(markdown-parser-static PUBLIC
    $<BUILD_INTERFACE:${markdown-parser_BINARY_DIR}/src>
    $<BUILD_INTERFACE:${markdown-parser_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:src>
    )

target_include_directories(markdown-parser-pic PUBLIC
    $<BUILD_INTERFACE:${markdown-parser_BINARY_DIR}/src>
    $<BUILD_INTERFACE:${markdown-parser_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:src>
    )

target_compile_features(markdown-parser PUBLIC ${MARKDOWN_PARSER_COMPILE_FEATURES})
target_compile_features(markdown-parser-static PUBLIC ${MARKDOWN_PARSER_COMPILE_FEATURES})
target_compile_features(markdown-parser-pic PUBLIC ${MARKDOWN_PARSER_COMPILE_FEATURES})

install(TARGETS markdown-parser markdown-parser-static markdown-parser-pic EXPORT markdown-parser-targets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
    )

install(EXPORT markdown-parser-targets
    FILE markdown-parser-targets.cmake
    NAMESPACE markdown-parser::
    DESTINATION lib/cmake/markdown-parser
    )

include(CMakePackageConfigHelpers)
write_basic_package_version_file("markdown-parser-config-version.cmake"
    VERSION ${markdown-parser_VERSION}
    COMPATIBILITY SameMajorVersion
    )

install(
    FILES
        "markdown-parser-config.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/markdown-parser-config-version.cmake"
    DESTINATION
        lib/cmake/markdown-parser
    )

add_library(markdown-parser::markdown-parser ALIAS markdown-parser)
add_library(markdown-parser::markdown-parser-static ALIAS markdown-parser-static)
add_library(markdown-parser::markdown-parser-pic ALIAS markdown-parser-pic)

if(BUILD_TESTING)
    include(markdown-parser-tests.cmake)
endif()

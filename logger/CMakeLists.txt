cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

project(Logger VERSION 1.0 LANGUAGES CXX)

set(Logger_SOURCES src/Trivial.cc)

add_library(logger SHARED ${Logger_SOURCES})
add_library(logger-static STATIC ${Logger_SOURCES})
add_library(logger-pic STATIC ${Logger_SOURCES})
set_property(TARGET logger-pic PROPERTY POSITION_INDEPENDENT_CODE 1)

if(UNIX)
    set_target_properties(logger-static PROPERTIES OUTPUT_NAME logger)
else()
    set_target_properties(logger-static PROPERTIES OUTPUT_NAME logger-static)
endif()

target_include_directories(logger PUBLIC 
    $<BUILD_INTERFACE:${Logger_BINARY_DIR}/include>
    $<BUILD_INTERFACE:${Logger_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    )
target_include_directories(logger-static PUBLIC 
    $<BUILD_INTERFACE:${Logger_BINARY_DIR}/include>
    $<BUILD_INTERFACE:${Logger_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    )
target_include_directories(logger-pic PUBLIC 
    $<BUILD_INTERFACE:${Logger_BINARY_DIR}/include>
    $<BUILD_INTERFACE:${Logger_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    )

target_link_libraries(logger)
target_link_libraries(logger-static)
target_link_libraries(logger-pic)

set(LOGGER_COMPILE_FEATURES
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

target_compile_features(logger PUBLIC ${LOGGER_COMPILE_FEATURES})
target_compile_features(logger-static PUBLIC ${LOGGER_COMPILE_FEATURES})
target_compile_features(logger-pic PUBLIC ${LOGGER_COMPILE_FEATURES})

install(TARGETS logger logger-static logger-pic EXPORT logger-targets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
    )

install(EXPORT logger-targets
    FILE LoggerTargets.cmake
    NAMESPACE Drafter::
    DESTINATION lib/cmake/drafter/logger
    )

include(CMakePackageConfigHelpers)
write_basic_package_version_file("LoggerConfigVersion.cmake"
    VERSION ${Logger_VERSION}
    COMPATIBILITY SameMajorVersion
    )
install(FILES
        "LoggerConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/LoggerConfigVersion.cmake"
    DESTINATION
        lib/cmake/drafter/logger
    )
install(DIRECTORY include/ DESTINATION include)

add_library(Drafter::logger ALIAS logger)
add_library(Drafter::logger-static ALIAS logger-static)
add_library(Drafter::logger-pic ALIAS logger-pic)


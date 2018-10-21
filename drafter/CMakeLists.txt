cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

project(Drafter VERSION 4.0 LANGUAGES CXX)

set(DRAFTER_SOURCES
    src/utils/so/Value.cc
    src/utils/so/JsonIo.cc
    src/utils/so/YamlIo.cc
    src/refract/dsd/Bool.cc
    src/refract/dsd/Member.cc
    src/refract/dsd/Object.cc
    src/refract/dsd/Ref.cc
    src/refract/dsd/Extend.cc
    src/refract/dsd/Holder.cc
    src/refract/dsd/Array.cc
    src/refract/dsd/Select.cc
    src/refract/dsd/Null.cc
    src/refract/dsd/Enum.cc
    src/refract/dsd/String.cc
    src/refract/dsd/Number.cc
    src/refract/dsd/Option.cc
    src/refract/Utils.cc
    src/refract/ElementUtils.cc
    src/refract/ComparableVisitor.cc
    src/refract/JsonValue.cc
    src/refract/JsonUtils.cc
    src/refract/SerializeSo.cc
    src/refract/Query.cc
    src/refract/InfoElements.cc
    src/refract/TypeQueryVisitor.cc
    src/refract/JsonSchema.cc
    src/refract/PrintVisitor.cc
    src/refract/Registry.cc
    src/refract/IsExpandableVisitor.cc
    src/refract/VisitorUtils.cc
    src/refract/ExpandVisitor.cc
    src/refract/Element.cc
    src/Serialize.cc
    src/SerializeResult.cc
    src/RefractAPI.cc
    src/drafter.cc
    src/RefractSourceMap.cc
    src/NamedTypesRegistry.cc
    src/RefractDataStructure.cc
    src/Render.cc
    src/ConversionContext.cc
    src/RefractElementFactory.cc
    )

set(DRAFTER_COMPILE_FEATURES
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

# libdrafter
add_library(drafter SHARED ${DRAFTER_SOURCES})
add_library(drafter-static STATIC ${DRAFTER_SOURCES})
add_library(drafter-pic STATIC ${DRAFTER_SOURCES})
set_property(TARGET drafter-pic PROPERTY POSITION_INDEPENDENT_CODE 1)

set_target_properties(drafter PROPERTIES PUBLIC_HEADER "src/drafter.h")

# production dependencies
find_package(snowcrash 1.0 REQUIRED)
find_package(BoostContainer 1.66 REQUIRED)
find_package(cmdline 1.0 REQUIRED)
find_package(Logger 1.0 REQUIRED)

target_link_libraries(drafter
    PRIVATE
        snowcrash::snowcrash-pic
        Boost::container-pic
        Drafter::logger-pic
    )

target_link_libraries(drafter-pic
    PRIVATE
        snowcrash::snowcrash-pic
        Boost::container-pic
        Drafter::logger-pic
    )

target_link_libraries(drafter-static
    PRIVATE
        snowcrash::snowcrash-static
        Boost::container-static
        Drafter::logger-static
    )

target_include_directories(drafter PUBLIC
    $<BUILD_INTERFACE:${Drafter_BINARY_DIR}/src>
    $<BUILD_INTERFACE:${Drafter_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:src>
    )

target_include_directories(drafter-static PUBLIC
    $<BUILD_INTERFACE:${Drafter_BINARY_DIR}/src>
    $<BUILD_INTERFACE:${Drafter_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:src>
    )

target_include_directories(drafter-pic PUBLIC
    $<BUILD_INTERFACE:${Drafter_BINARY_DIR}/src>
    $<BUILD_INTERFACE:${Drafter_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:src>
    )


target_compile_definitions(drafter PRIVATE BUILDING_DRAFTER=1)
target_compile_definitions(drafter-static PRIVATE BUILDING_DRAFTER=1)
target_compile_definitions(drafter-pic PRIVATE BUILDING_DRAFTER=1)

target_compile_definitions(drafter PUBLIC DRAFTER_BUILD_SHARED=1)

target_compile_features(drafter PUBLIC ${DRAFTER_COMPILE_FEATURES})
target_compile_features(drafter-static PUBLIC ${DRAFTER_COMPILE_FEATURES})
target_compile_features(drafter-pic PUBLIC ${DRAFTER_COMPILE_FEATURES})

# drafter-tui
add_executable(drafter-tui
    src/main.cc
    src/reporting.cc
    src/config.cc
    )

target_link_libraries(drafter-tui
    PRIVATE
        snowcrash::snowcrash-static
        drafter::drafter-static
        cmdline::cmdline
        Drafter::logger-static
    )

set_target_properties(drafter-tui PROPERTIES OUTPUT_NAME drafter)
if(UNIX)
        set_target_properties(drafter-static PROPERTIES OUTPUT_NAME drafter)
endif()

install(TARGETS drafter drafter-static drafter-pic drafter-tui EXPORT drafter-targets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
    PUBLIC_HEADER DESTINATION include
    )

# libdrafter tests
if(${BUILD_TESTING})
    include(drafter-tests.cmake)
endif()


install(EXPORT drafter-targets
    FILE drafter-targets.cmake
    NAMESPACE drafter::
    DESTINATION lib/cmake/drafter
    )

include(CMakePackageConfigHelpers)
write_basic_package_version_file("drafter-config-version.cmake"
    VERSION ${drafter_VERSION}
    COMPATIBILITY SameMajorVersion
    )

install(
    FILES
        "drafter-config.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/drafter-config-version.cmake"
    DESTINATION
        lib/cmake/drafter
    )

add_library(drafter::drafter ALIAS drafter)
add_library(drafter::drafter-static ALIAS drafter-static)
add_library(drafter::drafter-pic ALIAS drafter-pic)

 

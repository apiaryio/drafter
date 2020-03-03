cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

project(Drafter VERSION 5.0.0 LANGUAGES CXX)

set(DRAFTER_VERSION_IS_RELEASE 1)
set(DRAFTER_PRE_RELEASE_VERSION 0)

configure_file(Version.h.in Version.h)
include_directories(${PROJECT_BINARY_BIN})


set(DRAFTER_SOURCES
    ConversionContext.cc
    MsonMemberToApie.cc
    NamedTypesRegistry.cc
    RefractAPI.cc
    RefractDataStructure.cc
    RefractElementFactory.cc
    RefractSourceMap.cc
    Render.cc
    Serialize.cc
    SerializeKey.cc
    SerializeResult.cc
    SourceMapUtils.cc
    ContentTypeMatcher.cc
    options.cc
    refract/ComparableVisitor.cc
    refract/Element.cc
    refract/ElementSize.cc
    refract/ElementUtils.cc
    refract/ExpandVisitor.cc
    refract/InfoElements.cc
    refract/IsExpandableVisitor.cc
    refract/JsonSchema.cc
    refract/JsonUtils.cc
    refract/JsonValue.cc
    refract/PrintVisitor.cc
    refract/Query.cc
    refract/Registry.cc
    refract/SerializeSo.cc
    refract/TypeQueryVisitor.cc
    refract/Utils.cc
    refract/VisitorUtils.cc
    refract/dsd/Array.cc
    refract/dsd/Bool.cc
    refract/dsd/Enum.cc
    refract/dsd/Extend.cc
    refract/dsd/Holder.cc
    refract/dsd/Member.cc
    refract/dsd/Null.cc
    refract/dsd/Number.cc
    refract/dsd/Object.cc
    refract/dsd/Option.cc
    refract/dsd/Ref.cc
    refract/dsd/Select.cc
    refract/dsd/String.cc
    utils/log/Trivial.cc
    utils/so/JsonIo.cc
    utils/so/Value.cc
    utils/so/YamlIo.cc
    parser/Mediatype.cc
    format/MediatypeFmt.cc
    apib2apie/CollectionToApie.cc
    apib2apie/CopyToApie.cc
    apib2apie/LiteralToApie.cc
    apib2apie/ParametersToApie.cc
    apib2apie/PayloadToApie.cc
    apib2apie/PrimitiveToApie.cc
    apib2apie/SourceMapToApie.cc
    apib2apie/mson/OneOfToApie.cc
    apib2apie/mson/TypeToApie.cc
    )

set(DRAFTER_COMPILE_FEATURES
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

# production dependencies
find_package(snowcrash 1.0 REQUIRED)
find_package(BoostContainer 1.66 REQUIRED)
find_package(cmdline 1.0 REQUIRED)
find_package(MPark.Variant 1.4 REQUIRED)
find_package(pegtl 2.8 REQUIRED)

# drafter-dep 
# we need it to:
# - expose INTERFACE_INCLUDE_DIRECTORIES for drafter-obj
# - expose dependencies to drafter-lib, drafter-so, drafter-cli
#
# reason to have it, is drafter-obj does not allow to expose it via
# target_link_libraries() and target_include_directories() in version cmake 3.5
#
add_library(drafter-dep INTERFACE)
target_link_libraries(drafter-dep
    INTERFACE
    snowcrash::snowcrash
    Boost::container
    mpark_variant
    pegtl
    )
target_include_directories(drafter-dep
    INTERFACE 
    $<BUILD_INTERFACE:${Drafter_BINARY_DIR}>
    $<BUILD_INTERFACE:${Drafter_SOURCE_DIR}>
    )

# drafter-obj
add_library(drafter-obj OBJECT ${DRAFTER_SOURCES})
set_property(TARGET drafter-obj PROPERTY POSITION_INDEPENDENT_CODE 1)

target_include_directories(drafter-obj
    PRIVATE
    $<TARGET_PROPERTY:drafter-dep,INTERFACE_INCLUDE_DIRECTORIES>
    $<BUILD_INTERFACE:${Drafter_BINARY_DIR}>
    $<BUILD_INTERFACE:${Drafter_SOURCE_DIR}>
    )

target_compile_features(drafter-obj PUBLIC ${DRAFTER_COMPILE_FEATURES})

# drafter-lib
add_library(drafter-lib STATIC drafter.cc $<TARGET_OBJECTS:drafter-obj>)
set_target_properties(drafter-lib PROPERTIES OUTPUT_NAME drafter)
set_target_properties(drafter-lib PROPERTIES PUBLIC_HEADER "drafter.h" WINDOWS_EXPORT_ALL_SYMBOLS 1)
target_link_libraries(drafter-lib PUBLIC drafter-dep)

# FIXME: do we need this definitions
#target_compile_definitions(drafter-lib PRIVATE BUILDING_DRAFTER=1)

## drafter-cli
add_executable(drafter-cli
    main.cc
    reporting.cc
    config.cc
    )
set_target_properties(drafter-cli PROPERTIES OUTPUT_NAME drafter)
target_link_libraries(drafter-cli
    PRIVATE
    drafter-lib
    cmdline::cmdline
    )
#
# Windows build
# drafter.h -> 
# -> usedll !defined (DRAFTER_BUILD_SHARED) && !defined(DRAFTER_BUILD_STATIC) 
#
target_compile_definitions(drafter-lib PUBLIC DRAFTER_BUILD_STATIC=1)
#target_compile_definitions(drafter-cli PUBLIC DRAFTER_BUILD_STATIC=1)

set(EXPORTED_TARGETS drafter-dep drafter-lib drafter-cli)
set(DRAFTER_TARGETS drafter-cli)

if(NOT MSVC)

## drafter-so
add_library(drafter-so SHARED drafter.cc $<TARGET_OBJECTS:drafter-obj>) 
set_target_properties(drafter-so PROPERTIES OUTPUT_NAME drafter)
set_target_properties(drafter-so PROPERTIES PUBLIC_HEADER "drafter.h" WINDOWS_EXPORT_ALL_SYMBOLS 1)
target_link_libraries(drafter-so PUBLIC drafter-dep)
target_compile_definitions(drafter-so PUBLIC DRAFTER_BUILD_SHARED=1)

list(APPEND EXPORTED_TARGETS drafter-so)
list(APPEND DRAFTER_TARGETS drafter-so)

set_target_properties(drafter-so PROPERTIES SOVERSION ${PROJECT_VERSION})

endif()

add_custom_target(drafter DEPENDS ${DRAFTER_TARGETS})

# install
install(TARGETS ${EXPORTED_TARGETS} EXPORT drafter-targets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include/drafter
    PUBLIC_HEADER DESTINATION include/drafter
    )

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

add_library(drafter ALIAS drafter-lib)
add_library(drafter::drafter ALIAS drafter-lib)

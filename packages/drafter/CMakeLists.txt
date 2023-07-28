cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

project(Drafter VERSION 5.0.0 LANGUAGES CXX)

set(DRAFTER_VERSION_IS_RELEASE 1)

configure_file(src/Version.h.in src/Version.h)
include_directories(${PROJECT_BINARY_BIN})


set(DRAFTER_SOURCES
    src/ConversionContext.cc
    src/MsonOneOfSectionToApie.cc
    src/MsonTypeSectionToApie.cc
    src/NamedTypesRegistry.cc
    src/RefractAPI.cc
    src/RefractDataStructure.cc
    src/RefractElementFactory.cc
    src/RefractSourceMap.cc
    src/Render.cc
    src/Serialize.cc
    src/SerializeKey.cc
    src/SerializeResult.cc
    src/SourceMapUtils.cc
    src/options.cc
    src/refract/ComparableVisitor.cc
    src/refract/Element.cc
    src/refract/ElementSize.cc
    src/refract/ElementUtils.cc
    src/refract/ExpandVisitor.cc
    src/refract/InfoElements.cc
    src/refract/IsExpandableVisitor.cc
    src/refract/JsonSchema.cc
    src/refract/JsonUtils.cc
    src/refract/JsonValue.cc
    src/refract/PrintVisitor.cc
    src/refract/Query.cc
    src/refract/Registry.cc
    src/refract/SerializeSo.cc
    src/refract/TypeQueryVisitor.cc
    src/refract/Utils.cc
    src/refract/VisitorUtils.cc
    src/refract/dsd/Array.cc
    src/refract/dsd/Bool.cc
    src/refract/dsd/Enum.cc
    src/refract/dsd/Extend.cc
    src/refract/dsd/Holder.cc
    src/refract/dsd/Member.cc
    src/refract/dsd/Null.cc
    src/refract/dsd/Number.cc
    src/refract/dsd/Object.cc
    src/refract/dsd/Option.cc
    src/refract/dsd/Ref.cc
    src/refract/dsd/Select.cc
    src/refract/dsd/String.cc
    src/utils/log/Trivial.cc
    src/utils/so/JsonIo.cc
    src/utils/so/Value.cc
    src/utils/so/YamlIo.cc
    src/backend/MediaTypeS11n.cc
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
find_package(apib 1.0 REQUIRED)
find_package(apib-parser 1.0 REQUIRED)
find_package(BoostContainer 1.66 REQUIRED)
find_package(cmdline 1.0 REQUIRED)
find_package(MPark.Variant 1.4 REQUIRED)

add_definitions( -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} )

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
    Apiary::apib
    Apiary::apib-parser
    Boost::container
    mpark_variant
    )
target_include_directories(drafter-dep
    INTERFACE 
    $<BUILD_INTERFACE:${Drafter_BINARY_DIR}/src>
    $<BUILD_INTERFACE:${Drafter_SOURCE_DIR}/src>
    )

# drafter-obj
add_library(drafter-obj OBJECT ${DRAFTER_SOURCES})
set_property(TARGET drafter-obj PROPERTY POSITION_INDEPENDENT_CODE 1)

target_include_directories(drafter-obj
    PRIVATE
    $<TARGET_PROPERTY:drafter-dep,INTERFACE_INCLUDE_DIRECTORIES>
    $<BUILD_INTERFACE:${Drafter_BINARY_DIR}/src>
    $<BUILD_INTERFACE:${Drafter_SOURCE_DIR}/src>
    )

target_compile_features(drafter-obj PUBLIC ${DRAFTER_COMPILE_FEATURES})

# drafter-lib
add_library(drafter-lib STATIC src/drafter.cc $<TARGET_OBJECTS:drafter-obj>)
set_target_properties(drafter-lib PROPERTIES OUTPUT_NAME drafter)
set_target_properties(drafter-lib PROPERTIES PUBLIC_HEADER "src/drafter.h" WINDOWS_EXPORT_ALL_SYMBOLS 1)
target_link_libraries(drafter-lib PUBLIC drafter-dep)

# FIXME: do we need this definitions
#target_compile_definitions(drafter-lib PRIVATE BUILDING_DRAFTER=1)

## drafter-cli
add_executable(drafter-cli
    src/main.cc
    src/reporting.cc
    src/config.cc
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
add_library(drafter-so SHARED src/drafter.cc $<TARGET_OBJECTS:drafter-obj>) 
set_target_properties(drafter-so PROPERTIES OUTPUT_NAME drafter)
set_target_properties(drafter-so PROPERTIES PUBLIC_HEADER "src/drafter.h" WINDOWS_EXPORT_ALL_SYMBOLS 1)
target_link_libraries(drafter-so PUBLIC drafter-dep)
target_compile_definitions(drafter-so PUBLIC DRAFTER_BUILD_SHARED=1)

list(APPEND EXPORTED_TARGETS drafter-so)
list(APPEND DRAFTER_TARGETS drafter-so)

set_target_properties(drafter-so PROPERTIES SOVERSION ${PROJECT_VERSION})

endif()

add_custom_target(drafter DEPENDS ${DRAFTER_TARGETS})

# install
install(TARGETS ${EXPORTED_TARGETS} EXPORT drafter-targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/drafter
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/drafter
    )

install(EXPORT drafter-targets
    FILE drafter-targets.cmake
    NAMESPACE drafter::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/drafter
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
        ${CMAKE_INSTALL_LIBDIR}/cmake/drafter
    )

add_library(drafter ALIAS drafter-lib)
add_library(drafter::drafter ALIAS drafter-lib)

if(${BUILD_TESTING})
    option(INTEGRATION_TESTS "Run integration tests" OFF)
    add_subdirectory(test)
    if(${INTEGRATION_TESTS})
        include(integration.cmake)
    endif()
    add_custom_target(drafter-test-suite ALL)
    add_dependencies(drafter-test-suite drafter-test apib-parser-test apib-test)
endif()

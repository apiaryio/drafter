cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

project(Drafter VERSION 4.0 LANGUAGES CXX)

set(DRAFTER_SOURCES
    ConversionContext.cc
    MsonMemberToApie.cc
    MsonOneOfSectionToApie.cc
    MsonTypeSectionToApie.cc
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
    drafter.cc
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

# libdrafter
add_library(drafter ${DRAFTER_SOURCES})

set_target_properties(drafter PROPERTIES PUBLIC_HEADER "drafter.h" WINDOWS_EXPORT_ALL_SYMBOLS 1)

# production dependencies
find_package(snowcrash 1.0 REQUIRED)
find_package(BoostContainer 1.66 REQUIRED)
find_package(cmdline 1.0 REQUIRED)
find_package(MPark.Variant 1.4 REQUIRED)
find_package(pegtl 2.8 REQUIRED)

target_link_libraries(drafter
    PRIVATE
        snowcrash::snowcrash
        Boost::container
        mpark_variant
        taocpp::pegtl
    )

target_include_directories(drafter PUBLIC
    $<BUILD_INTERFACE:${Drafter_BINARY_DIR}>
    $<BUILD_INTERFACE:${Drafter_SOURCE_DIR}>
    $<INSTALL_INTERFACE:>
    )

target_compile_definitions(drafter PRIVATE BUILDING_DRAFTER=1)

target_compile_definitions(drafter PUBLIC DRAFTER_BUILD_STATIC=1)

target_compile_features(drafter PUBLIC ${DRAFTER_COMPILE_FEATURES})

# drafter-cli
add_executable(drafter-cli
    main.cc
    reporting.cc
    config.cc
    )

target_link_libraries(drafter-cli
    PRIVATE
        snowcrash::snowcrash
        drafter::drafter
        cmdline::cmdline
        mpark_variant
        taocpp::pegtl
    )

set_target_properties(drafter-cli PROPERTIES OUTPUT_NAME drafter)

install(TARGETS drafter drafter-cli EXPORT drafter-targets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
    PUBLIC_HEADER DESTINATION include
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

add_library(drafter::drafter ALIAS drafter)

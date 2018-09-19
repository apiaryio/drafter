cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

project(drafter VERSION 0.1 LANGUAGES CXX)

add_library(drafter
    src/utils/so/Value.cc
    src/utils/so/JsonIo.cc
    src/utils/so/YamlIo.cc
    src/utils/log/Trivial.cc
    src/reporting.cc
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
    src/config.cc
    src/drafter.cc
    src/RefractSourceMap.cc
    src/NamedTypesRegistry.cc
    src/RefractDataStructure.cc
    src/Render.cc
    src/ConversionContext.cc
    src/RefractElementFactory.cc
    )

add_executable(drafter-tui src/main.cc)
set_target_properties(drafter-tui PROPERTIES OUTPUT_NAME drafter)

find_package(snowcrash 1.0 REQUIRED)
find_package(Boost 1.66 REQUIRED)
find_package(cmdline 1.0 REQUIRED)

target_link_libraries(drafter
    PUBLIC
        snowcrash::snowcrash
    PRIVATE
        cmdline::cmdline # TODO @tjanc@ should be a dep of drafter-tui
    )

target_link_libraries(drafter-tui
    PRIVATE
        drafter::drafter
    )

target_include_directories(drafter
    PUBLIC
        $<BUILD_INTERFACE:${drafter_BINARY_DIR}/src>
        $<BUILD_INTERFACE:${drafter_SOURCE_DIR}/src>
        $<INSTALL_INTERFACE:src>
    )

target_compile_features(drafter PUBLIC cxx_std_14)

install(TARGETS drafter EXPORT drafter-targets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
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
install(FILES "drafter-config.cmake" "drafter-config-version.cmake"
    DESTINATION lib/cmake/drafter
    )

add_library(drafter::drafter ALIAS drafter)

# tests
find_package(Catch2 1.0 REQUIRED)

add_executable(drafter-test
    test/utils/test-Utf8.cc
    test/utils/so/test-YamlIo.cc
    test/utils/so/test-JsonIo.cc
    test/utils/test-Variant.cc
    test/test-RefractAPITest.cc
    test/test-ElementComparator.cc
    test/refract/dsd/test-Option.cc
    test/refract/dsd/test-Object.cc
    test/refract/dsd/test-Select.cc
    test/refract/dsd/test-Extend.cc
    test/refract/dsd/test-String.cc
    test/refract/dsd/test-Holder.cc
    test/refract/dsd/test-Element.cc
    test/refract/dsd/test-Array.cc
    test/refract/dsd/test-Number.cc
    test/refract/dsd/test-Ref.cc
    test/refract/dsd/test-InfoElements.cc
    test/refract/dsd/test-Null.cc
    test/refract/dsd/test-Bool.cc
    test/refract/dsd/test-Member.cc
    test/refract/dsd/test-Enum.cc
    test/refract/test-InfoElementsUtils.cc
    test/refract/test-Utils.cc
    test/refract/test-JsonSchema.cc
    test/test-VisitorUtils.cc
    test/test-SyntaxIssuesTest.cc
    test/test-ApplyVisitorTest.cc
    test/test-ElementDataTest.cc
    test/test-RefractDataStructureTest.cc
    test/test-ElementInfoUtils.cc
    test/test-drafter.cc
    test/test-ExtendElementTest.cc
    test/test-ElementFactoryTest.cc
    test/test-SchemaTest.cc
    test/test-OneOfTest.cc
    test/test-RefractParseResultTest.cc
    test/test-RefractSourceMapTest.cc
    test/test-CircularReferenceTest.cc
    test/test-RenderTest.cc
    )

target_link_libraries(drafter-test
    PRIVATE
        Catch2::Catch2
        dtl::dtl
        drafter::drafter
    )

catch_discover_tests(drafter-test)

file(
    COPY
        ${CMAKE_CURRENT_SOURCE_DIR}/test/fixtures/
    DESTINATION
        ${CMAKE_CURRENT_BINARY_DIR}/test/fixtures/
    )


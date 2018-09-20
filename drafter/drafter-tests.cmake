cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

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
    test/refract/test-JsonValue.cc
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
    test/test-Serialize.cc
    )

target_link_libraries(drafter-test
    PRIVATE
        Catch2::Catch2
        dtl::dtl
        drafter::drafter-static
        snowcrash::snowcrash-static
        Boost::container-static
    )

catch_discover_tests(drafter-test)

file(
    COPY
        ${CMAKE_CURRENT_SOURCE_DIR}/test/fixtures/
    DESTINATION
        ${CMAKE_CURRENT_BINARY_DIR}/test/fixtures/
    )


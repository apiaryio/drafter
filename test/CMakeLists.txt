cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

find_package(Catch2 1.0 REQUIRED)
find_package(MPark.Variant 1.4 REQUIRED)

add_executable(drafter-test
    utils/test-Utf8.cc
    utils/so/test-YamlIo.cc
    utils/so/test-JsonIo.cc
    test-RefractAPITest.cc
    test-ElementComparator.cc
    refract/dsd/test-Option.cc
    refract/dsd/test-Object.cc
    refract/dsd/test-Select.cc
    refract/dsd/test-Extend.cc
    refract/dsd/test-String.cc
    refract/dsd/test-Holder.cc
    refract/dsd/test-Element.cc
    refract/dsd/test-Array.cc
    refract/dsd/test-Number.cc
    refract/dsd/test-Ref.cc
    refract/dsd/test-InfoElements.cc
    refract/dsd/test-Null.cc
    refract/dsd/test-Bool.cc
    refract/dsd/test-Member.cc
    refract/dsd/test-Enum.cc
    refract/test-Cardinal.cc
    refract/test-ElementSize.cc
    refract/test-InfoElementsUtils.cc
    refract/test-JsonSchema.cc
    refract/test-JsonValue.cc
    refract/test-Utils.cc
    test-VisitorUtils.cc
    test-SyntaxIssuesTest.cc
    test-ApplyVisitorTest.cc
    test-ElementDataTest.cc
    test-RefractDataStructureTest.cc
    test-ElementInfoUtils.cc
    test-drafter.cc
    test-ExtendElementTest.cc
    test-ElementFactoryTest.cc
    test-SchemaTest.cc
    test-OneOfTest.cc
    test-RefractParseResultTest.cc
    test-RefractSourceMapTest.cc
    test-CircularReferenceTest.cc
    test-RenderTest.cc
    test-Serialize.cc
    test-sourceMapToLineColumn.cc
    parser/test-MediaType.cc
    test-ContentTypeMatcher.cc
    )

target_link_libraries(drafter-test
    PRIVATE
        Catch2::Catch2
        dtl::dtl
        drafter::drafter
        snowcrash::snowcrash
        Boost::container
        mpark_variant
    )

add_test(DrafterTest drafter-test)

file(
    COPY
        ${CMAKE_CURRENT_SOURCE_DIR}/fixtures/
    DESTINATION
        ${CMAKE_CURRENT_BINARY_DIR}/test/fixtures/
    )

add_executable(drafter-ctest
    test-CAPI.c
    )

target_link_libraries(drafter-ctest
    PRIVATE
        drafter::drafter
    )

add_test(DrafterCTest drafter-ctest)

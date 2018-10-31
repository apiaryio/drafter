cmake_minimum_required(VERSION 3.4 FATAL_ERROR)

find_package(Catch2 1.0 REQUIRED)

add_executable(snowcrash-test
    test/test-Blueprint.cc
    test/test-MSONUtility.cc
    test/test-MSONNamedTypeParser.cc
    test/test-ModelTable.cc
    test/test-Signature.cc
    test/test-ParametersParser.cc
    test/test-StringUtility.cc
    test/test-snowcrash.cc
    test/test-MSONOneOfParser.cc
    test/test-Indentation.cc
    test/test-UriTemplateParser.cc
    test/test-PayloadParser.cc
    test/test-SymbolIdentifier.cc
    test/test-RegexMatch.cc
    test/test-HeadersParser.cc
    test/test-MSONValueMemberParser.cc
    test/test-DataStructureGroupParser.cc
    test/test-MSONPropertyMemberParser.cc
    test/test-ParameterParser.cc
    test/test-MSONTypeSectionParser.cc
    test/test-MSONParameterParser.cc
    test/test-RelationParser.cc
    test/test-SectionParser.cc
    test/test-MSONMixinParser.cc
    test/test-ValuesParser.cc
    test/test-BlueprintUtility.cc
    test/test-Warnings.cc
    test/test-ResourceParser.cc
    test/test-ResourceGroupParser.cc
    test/test-BlueprintParser.cc
    test/test-AssetParser.cc
    test/test-ActionParser.cc
    test/test-AttributesParser.cc
    )

add_executable(snowcrash-test-performance
    test/performance/perf-snowcrash.cc
    )

catch_discover_tests(snowcrash-test)

target_link_libraries(snowcrash-test
    PRIVATE
        Catch2::Catch2
        snowcrash::snowcrash-static
        markdown-parser::markdown-parser-static
    )

target_link_libraries(snowcrash-test-performance
    PRIVATE
        snowcrash::snowcrash
    )

file(
    COPY
        ${CMAKE_CURRENT_SOURCE_DIR}/test/performance/
    DESTINATION
        ${CMAKE_CURRENT_BINARY_DIR}/test/performance/
    )


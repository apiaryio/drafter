cmake_minimum_required(VERSION 3.4 FATAL_ERROR)

find_package(Catch2 1.0 REQUIRED)

add_executable(apib-parser-test
    test-apib_parser.cc
    grammar/test-MediaType.cc
    grammar/test-UriTemplate.cc
    grammar/test-Recover.cc
    snowcrash/test-Blueprint.cc
    snowcrash/test-MSONUtility.cc
    snowcrash/test-MSONNamedTypeParser.cc
    snowcrash/test-ModelTable.cc
    snowcrash/test-Signature.cc
    snowcrash/test-ParametersParser.cc
    snowcrash/test-StringUtility.cc
    snowcrash/test-snowcrash.cc
    snowcrash/test-MSONOneOfParser.cc
    snowcrash/test-Indentation.cc
    snowcrash/test-UriTemplateParser.cc
    snowcrash/test-PayloadParser.cc
    snowcrash/test-SymbolIdentifier.cc
    snowcrash/test-RegexMatch.cc
    snowcrash/test-HeadersParser.cc
    snowcrash/test-MSONValueMemberParser.cc
    snowcrash/test-DataStructureGroupParser.cc
    snowcrash/test-MSONPropertyMemberParser.cc
    snowcrash/test-ParameterParser.cc
    snowcrash/test-MSONTypeSectionParser.cc
    snowcrash/test-MSONParameterParser.cc
    snowcrash/test-RelationParser.cc
    snowcrash/test-SectionParser.cc
    snowcrash/test-MSONMixinParser.cc
    snowcrash/test-ValuesParser.cc
    snowcrash/test-BlueprintUtility.cc
    snowcrash/test-Warnings.cc
    snowcrash/test-ResourceParser.cc
    snowcrash/test-ResourceGroupParser.cc
    snowcrash/test-BlueprintParser.cc
    snowcrash/test-AssetParser.cc
    snowcrash/test-ActionParser.cc
    snowcrash/test-AttributesParser.cc
    markdown-parser/test-ByteBuffer.cc
    markdown-parser/test-MarkdownParser.cc
    )

add_executable(apib-parser-test-performance
    snowcrash/performance/perf-snowcrash.cc
    )

add_test(ApibParserTest apib-parser-test)

target_link_libraries(apib-parser-test
    PRIVATE
        Catch2::Catch2
        Apiary::apib-parser
        pegtl
    )

target_include_directories(apib-parser-test PRIVATE ../src)

target_link_libraries(apib-parser-test-performance
    PRIVATE
        Apiary::apib-parser
    )

file(
    COPY
        ${CMAKE_CURRENT_SOURCE_DIR}/snowcrash/performance/
    DESTINATION
        ${CMAKE_CURRENT_BINARY_DIR}/snowcrash/performance/
    )


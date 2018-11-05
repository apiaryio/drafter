cmake_minimum_required(VERSION 3.4 FATAL_ERROR)

find_package(Catch2 1.0 REQUIRED)

add_executable(markdown-parser-test
    test/test-ByteBuffer.cc
    test/test-libmarkdownparser.cc
    test/test-MarkdownParser.cc
    )

if(MONOLITHIC_TESTS)
    add_test(MarkdownParserTest markdown-parser-test)
else()
    catch_discover_tests(markdown-parser-test)
endif()

target_link_libraries(markdown-parser-test
    PRIVATE
        Catch2::Catch2
        markdown-parser::markdown-parser-static
    )


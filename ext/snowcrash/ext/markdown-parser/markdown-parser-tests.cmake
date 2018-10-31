cmake_minimum_required(VERSION 3.4 FATAL_ERROR)

find_package(Catch2 1.0 REQUIRED)

add_executable(markdown-parser-test
    test/test-ByteBuffer.cc
    test/test-libmarkdownparser.cc
    test/test-MarkdownParser.cc
    )

catch_discover_tests(markdown-parser-test)

target_link_libraries(markdown-parser-test
    PRIVATE
        Catch2::Catch2
        markdown-parser::markdown-parser-static
    )


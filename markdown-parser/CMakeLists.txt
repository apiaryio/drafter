cmake_minimum_required(VERSION 3.4 FATAL_ERROR)

project(markdown-parser VERSION 0.1 LANGUAGES CXX)

add_library(markdown-parser
      src/ByteBuffer.cc
      src/MarkdownNode.cc
      src/MarkdownParser.cc
    )

find_package(sundown-apiary 1.0 REQUIRED)
target_link_libraries(markdown-parser PUBLIC sundown-apiary::sundown-apiary)

target_include_directories(markdown-parser PUBLIC
    $<BUILD_INTERFACE:${markdown-parser_BINARY_DIR}/src>
    $<BUILD_INTERFACE:${markdown-parser_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:src>
    )

install(TARGETS markdown-parser EXPORT markdown-parser-targets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
    )

install(EXPORT markdown-parser-targets
    FILE markdown-parser-targets.cmake
    NAMESPACE markdown-parser::
    DESTINATION lib/cmake/markdown-parser
    )

include(CMakePackageConfigHelpers)
write_basic_package_version_file("markdown-parser-config-version.cmake"
    VERSION ${markdown-parser_VERSION}
    COMPATIBILITY SameMajorVersion
    )
install(FILES "markdown-parser-config.cmake" "markdown-parser-config-version.cmake"
    DESTINATION lib/cmake/markdown-parser
    )

add_library(markdown-parser::markdown-parser ALIAS markdown-parser)

# tests
find_package(Catch2 1.0 REQUIRED)

add_executable(markdown-parser-test
    test/test-ByteBuffer.cc
    test/test-libmarkdownparser.cc
    test/test-MarkdownParser.cc
    )

add_test(markdown-parser markdown-parser-test)

target_link_libraries(markdown-parser-test
    PRIVATE
        Catch2::Catch2
        markdown-parser::markdown-parser
    )


cmake_minimum_required(VERSION 3.4 FATAL_ERROR)

project(snowcrash VERSION 0.1 LANGUAGES CXX)

set(snowcrash_SOURCES
      src/Blueprint.cc
      src/BlueprintSourcemap.cc
      src/HeadersParser.cc
      src/HTTP.cc
      src/MSON.cc
      src/MSONOneOfParser.cc
      src/MSONSourcemap.cc
      src/MSONTypeSectionParser.cc
      src/MSONValueMemberParser.cc
      src/Section.cc
      src/Signature.cc
      src/snowcrash.cc
      src/UriTemplateParser.cc
    )

if(MSVC)
    set(snowcrash_SOURCES ${snowcrash_SOURCES} src/win/RegexMatch.cc)
else()
    set(snowcrash_SOURCES ${snowcrash_SOURCES} src/posix/RegexMatch.cc)
endif()

add_library(snowcrash ${snowcrash_SOURCES})

find_package(markdown-parser 1.0 REQUIRED)

target_link_libraries(snowcrash PUBLIC markdown-parser::markdown-parser)

target_include_directories(snowcrash PUBLIC
    $<BUILD_INTERFACE:${snowcrash_BINARY_DIR}/src>
    $<BUILD_INTERFACE:${snowcrash_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:src>
    )

install(TARGETS snowcrash EXPORT snowcrash-targets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
    )

install(EXPORT snowcrash-targets
    FILE snowcrash-targets.cmake
    NAMESPACE snowcrash::
    DESTINATION lib/cmake/snowcrash
    )

include(CMakePackageConfigHelpers)
write_basic_package_version_file("snowcrash-config-version.cmake"
    VERSION ${snowcrash_VERSION}
    COMPATIBILITY SameMajorVersion
    )
install(FILES "snowcrash-config.cmake" "snowcrash-config-version.cmake"
    DESTINATION lib/cmake/snowcrash
    )

add_library(snowcrash::snowcrash ALIAS snowcrash)

# tests
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

add_test(snowcrash snowcrash-test)
# add_test(snowcrash-performance snowcrash-test-performance)

target_link_libraries(snowcrash-test
    PRIVATE
        Catch2::Catch2
        snowcrash::snowcrash
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


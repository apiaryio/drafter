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
install(
    FILES
        "snowcrash-config.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/snowcrash-config-version.cmake"
    DESTINATION
        lib/cmake/snowcrash
    )

add_library(snowcrash::snowcrash ALIAS snowcrash)

if(BUILD_TESTING)
    include(snowcrash-tests.cmake)
endif()

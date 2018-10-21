cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

project(Logger VERSION 1.0 LANGUAGES CXX)

set(Logger_SOURCES src/Trivial.cc)

add_library(logger SHARED ${Logger_SOURCES})
add_library(logger-static STATIC ${Logger_SOURCES})
add_library(logger-pic STATIC ${Logger_SOURCES})
set_property(TARGET logger-pic PROPERTY POSITION_INDEPENDENT_CODE 1)

if(UNIX)
    set_target_properties(logger-static PROPERTIES OUTPUT_NAME logger)
else()
    set_target_properties(logger-static PROPERTIES OUTPUT_NAME logger-static)
endif()

set_target_properties(logger PROPERTIES OUTPUT_NAME logger)
set_target_properties(logger-pic PROPERTIES OUTPUT_NAME logger-pic)

target_include_directories(logger PUBLIC 
    $<BUILD_INTERFACE:${Logger_BINARY_DIR}/include>
    $<BUILD_INTERFACE:${Logger_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    )
target_include_directories(logger-static PUBLIC 
    $<BUILD_INTERFACE:${Logger_BINARY_DIR}/include>
    $<BUILD_INTERFACE:${Logger_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    )
target_include_directories(logger-pic PUBLIC 
    $<BUILD_INTERFACE:${Logger_BINARY_DIR}/include>
    $<BUILD_INTERFACE:${Logger_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    )

target_link_libraries(logger)
target_link_libraries(logger-static)
target_link_libraries(logger-pic)


install(TARGETS logger logger-static logger-pic EXPORT logger-targets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
    )

install(EXPORT logger-targets
    FILE LoggerTargets.cmake
    NAMESPACE Drafter::
    DESTINATION lib/cmake/drafter/logger
    )

include(CMakePackageConfigHelpers)
write_basic_package_version_file("LoggerConfigVersion.cmake"
    VERSION ${Logger_VERSION}
    COMPATIBILITY SameMajorVersion
    )
install(FILES
        "LoggerConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/LoggerConfigVersion.cmake"
    DESTINATION
        lib/cmake/drafter/logger
    )
install(DIRECTORY include/ DESTINATION include)

add_library(Drafter::logger ALIAS logger)
add_library(Drafter::logger-static ALIAS logger-static)
add_library(Drafter::logger-pic ALIAS logger-pic)


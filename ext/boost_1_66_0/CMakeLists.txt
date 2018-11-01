cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

project(BoostContainer VERSION 1.66 LANGUAGES CXX)

add_library(container INTERFACE)

target_include_directories(container INTERFACE 
    $<BUILD_INTERFACE:${BoostContainer_BINARY_DIR}>
    $<BUILD_INTERFACE:${BoostContainer_SOURCE_DIR}>
    $<INSTALL_INTERFACE:>
    )

target_link_libraries(container INTERFACE)

install(EXPORT container-targets
    FILE BoostContainerTargets.cmake
    NAMESPACE Boost::
    DESTINATION lib/cmake/boost
    )

install(TARGETS container EXPORT container-targets
    INCLUDES DESTINATION include
    )

include(CMakePackageConfigHelpers)
write_basic_package_version_file("BoostContainerConfigVersion.cmake"
    VERSION ${BoostContainer_VERSION}
    COMPATIBILITY SameMajorVersion
    )
install(
    FILES
        "BoostContainerConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/BoostContainerConfigVersion.cmake"
    DESTINATION
        lib/cmake/boost
    )

add_library(Boost::container ALIAS container)

cmake_minimum_required(VERSION 3.5)
set(CTEST_PROJECT_NAME "Drafter")
if("${CTEST_BUILD_NAME}" STREQUAL "")
    set(CTEST_BUILD_NAME "Unix/${CTEST_BUILD_TYPE}")
else()
    set(CTEST_BUILD_NAME "Unix/${CTEST_BUILD_TYPE}/${CTEST_BUILD_NAME}")
endif()

set(CTEST_SOURCE_DIRECTORY ".")
set(CTEST_BINARY_DIRECTORY "build")

set(CTEST_CMAKE_GENERATOR "Unix Makefiles")

set(CTEST_USE_LAUNCHERS 1)

ctest_start("Continuous")
ctest_configure(OPTIONS "-DINTEGRATION_TESTS=ON")
ctest_build(FLAGS -j5 TARGET drafter-test-suite)
ctest_test()
ctest_submit()

cmake_minimum_required(VERSION 3.6)
set(CTEST_PROJECT_NAME "Drafter")
set(CTEST_BUILD_NAME "GNU/coverage")

set(CTEST_SOURCE_DIRECTORY ".")
set(CTEST_BINARY_DIRECTORY "build")

set(CTEST_CMAKE_GENERATOR "Unix Makefiles")

set(CTEST_USE_LAUNCHERS 1)

set(CTEST_COVERAGE_COMMAND "/usr/bin/gcov")

set(Drafter_CONFIG_OPTIONS
    "-DCMAKE_CXX_FLAGS=--coverage"
    "-DINTEGRATION_TESTS=ON"
    )

ctest_start("Continuous")
ctest_configure(OPTIONS "${Drafter_CONFIG_OPTIONS}")
ctest_build(FLAGS -j5 TARGET drafter-test-suite)
ctest_test()
ctest_coverage(CAPTURE_CMAKE_ERROR COVERAGE_ERROR)
ctest_submit()

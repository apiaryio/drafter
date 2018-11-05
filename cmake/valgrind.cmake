cmake_minimum_required(VERSION 3.5)
set(CTEST_PROJECT_NAME "Drafter")
set(CTEST_BUILD_NAME "GNU/valgrind")

set(CTEST_SOURCE_DIRECTORY ".")
set(CTEST_BINARY_DIRECTORY "build")

set(CTEST_CMAKE_GENERATOR "Unix Makefiles")

set(CTEST_USE_LAUNCHERS 1)

set(CTEST_MEMORYCHECK_COMMAND "/usr/bin/valgrind")

ctest_start("Continuous")
ctest_configure(OPTIONS "-DMONOLITHIC_TESTS=ON")
ctest_build(FLAGS -j5 TARGET drafter-test-suite)
ctest_memcheck(EXCLUDE DrafterIntegration)
ctest_submit()

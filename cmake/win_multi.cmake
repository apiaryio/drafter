cmake_minimum_required(VERSION 3.5)
set(CTEST_PROJECT_NAME "Drafter")
set(CTEST_BUILD_NAME "Win/${CTEST_BUILD_TYPE}")

set(CTEST_SOURCE_DIRECTORY ".")
set(CTEST_BINARY_DIRECTORY "build")

set(CTEST_USE_LAUNCHERS 1)

ctest_start("Continuous")
ctest_configure()
ctest_build(TARGET drafter-test-suite)
ctest_test()
ctest_submit()

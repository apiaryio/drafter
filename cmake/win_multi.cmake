cmake_minimum_required(VERSION 3.5)
set(CTEST_PROJECT_NAME "Drafter")
set(CTEST_BUILD_NAME "Win/${CTEST_BUILD_TYPE}")

set(CTEST_SOURCE_DIRECTORY ".")
set(CTEST_BINARY_DIRECTORY "build")

set(CTEST_USE_LAUNCHERS 1)

set(Drafter_CONFIG_OPTIONS "-DINTEGRATION_TESTS=ON")

ctest_start("Continuous")
ctest_configure(OPTIONS "${Drafter_CONFIG_OPTIONS}")
ctest_build(TARGET drafter-test-suite)
ctest_test()
ctest_submit()

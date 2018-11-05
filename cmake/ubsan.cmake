cmake_minimum_required(VERSION 3.6)
set(CTEST_PROJECT_NAME "Drafter")
set(CTEST_BUILD_NAME "GNU/ubsan")

set(CTEST_SOURCE_DIRECTORY ".")
set(CTEST_BINARY_DIRECTORY "build")

set(CTEST_CMAKE_GENERATOR "Unix Makefiles")
set(CTEST_MEMORYCHECK_TYPE UndefinedBehaviorSanitizer)
set(CTEST_MEMORYCHECK_SANITIZER_OPTIONS "print_stacktrace=1:halt_on_error=1")

set(CTEST_USE_LAUNCHERS 1)

set(Drafter_CONFIG_OPTIONS
    "-DCMAKE_CXX_FLAGS='-fsanitize=undefined'"
    )

ctest_start("Continuous")
ctest_configure(OPTIONS "${Drafter_CONFIG_OPTIONS}")
ctest_build(FLAGS -j5 TARGET drafter-test-suite)
ctest_memcheck(EXCLUDE DrafterIntegration)
ctest_submit()

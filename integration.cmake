cmake_minimum_required(VERSION 3.6 FATAL_ERROR)

find_program(Bundle bundle)
add_test(NAME DrafterIntegration COMMAND ${Bundle} exec cucumber .)
set_property(TEST DrafterIntegration PROPERTY ENVIRONMENT "PATH=$ENV{PATH};${Drafter_BUILD_DIR}")


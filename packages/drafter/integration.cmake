cmake_minimum_required(VERSION 3.6 FATAL_ERROR)

find_program(Cucumber cucumber)
add_test(NAME DrafterIntegration COMMAND ${Cucumber} .)
set_property(TEST DrafterIntegration PROPERTY ENVIRONMENT "PATH=$ENV{PATH};${Drafter_BUILD_DIR}")


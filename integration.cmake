cmake_minimum_required(VERSION 3.6 FATAL_ERROR)

find_program(Bundler bundle)

file(COPY ${CMAKE_CURRENT_LIST_DIR}/features DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
add_custom_target(BundleInstall ALL COMMAND ${Bundler} install --path vendor/bundle)
add_test(NAME DrafterIntegration COMMAND ${Bundler} exec cucumber)

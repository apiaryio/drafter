cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

find_package(Catch2 1.0 REQUIRED)

add_executable(apib-test
    test-MediaType.cc
    test-apib.cc
    )

target_link_libraries(apib-test
    PUBLIC
        Catch2::Catch2
        Apiary::apib
    )

target_include_directories(apib-test PRIVATE ../src)

add_test(ApibTest apib-test)

cmake_minimum_required(VERSION 3.20)
project(spc_test)

add_subdirectory(gtest)
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

set(TEST_SOURCE_FILES stats_test.cc)

add_executable(spc_test ${TEST_SOURCE_FILES})

target_link_directories(spc_test PRIVATE ${CMAKE_BINARY_DIR})

target_link_libraries(spc_test PRIVATE gtest gtest_main)
target_link_libraries(spc_test PRIVATE spc_lib)

target_include_directories(spc_test
        PRIVATE
        ../include)

include(GoogleTest)
gtest_add_tests(spc_test "" ${TEST_SOURCE_FILES})
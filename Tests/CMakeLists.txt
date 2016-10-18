cmake_minimum_required(VERSION 3.2 FATAL_ERROR)

include(ImportGTest)

add_executable(TestMultiIndex testMultiIndex.cpp)
target_link_libraries(TestMultiIndex PRIVATE ImageStack GTest)
add_test(TestMultiIndex TestMultiIndex)


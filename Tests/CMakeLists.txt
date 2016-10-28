cmake_minimum_required(VERSION 3.2 FATAL_ERROR)

set(TEST_DATA_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../Data")

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/config.h.in
  ${CMAKE_CURRENT_BINARY_DIR}/config.h
)

include_directories("${CMAKE_CURRENT_BINARY_DIR}")

add_executable(TestMultiIndex testMultiIndex.cpp)
target_link_libraries(TestMultiIndex PRIVATE ImageStack GTest)
add_test(TestMultiIndex TestMultiIndex)

add_executable(TestMappedMemory testMappedMemory.cpp)
target_link_libraries(TestMappedMemory PRIVATE ImageStack GTest)
add_test(TestMappedMemory TestMappedMemory)

add_executable(TestHostStorage testHostStorage.cpp)
target_link_libraries(TestHostStorage PRIVATE ImageStack GTest)
add_test(TestHostStorage TestHostStorage)

add_executable(TestImageStack testImageStack.cpp)
target_link_libraries(TestImageStack PRIVATE ImageStack GTest)
add_test(TestImageStack TestImageStack)

add_executable(TestBSTLoader testBSTLoader.cpp)
target_link_libraries(TestBSTLoader PRIVATE ImageStack GTest)
add_test(TestBSTLoader TestBSTLoader)

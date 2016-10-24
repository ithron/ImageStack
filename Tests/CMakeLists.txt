cmake_minimum_required(VERSION 3.2 FATAL_ERROR)

include(ImportGTest)

add_executable(TestMultiIndex testMultiIndex.cpp)
target_link_libraries(TestMultiIndex PRIVATE ImageStack GTest)
add_test(TestMultiIndex TestMultiIndex)

add_executable(TestMappedMemory testMappedMemory.cpp)
target_link_libraries(TestMappedMemory PRIVATE ImageStack GTest)
add_test(TestMappedMemory TestMappedMemory)

add_executable(TestHostStorage testHostStorage.cpp)
target_link_libraries(TestHostStorage PRIVATE ImageStack GTest)
add_test(TestHostStorage TestHostStorage)

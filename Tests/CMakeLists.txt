
set(BUILD_TYPE ${CMAKE_BUILD_TYPE})

set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build googletest as release build")
set(BUILD_GTEST ON CACHE BOOL "Build googletest")
set(BUILD_GMOCK OFF CACHE BOOL "Build googlemock")
add_subdirectory(${CMAKE_SOURCE_DIR}/Dependencies/googletest googletest)
set(CMAKE_BUILD_TYPE ${BUILD_TYPE})

add_executable(TestBSTLoader Source/testBSTLoader.cpp)
target_link_libraries(TestBSTLoader PRIVATE ImageStack gtest_main)

add_test(TestBSTLoader TestBSTLoader)


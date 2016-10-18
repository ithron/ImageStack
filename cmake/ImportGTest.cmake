cmake_minimum_required(VERSION 3.2 FATAL_ERROR)

set(BUILD_TYPE ${CMAKE_BUILD_TYPE})

set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build gtest as release build")
set(BUILD_GMOCK OFF CACHE STRING "Don't build gmock")
set(BUILD_GTEST ON CACHE STRING "Build gtest")

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_subdirectory(${CMAKE_SOURCE_DIR}/Dependencies/googletest googletest EXCLUDE_FROM_ALL)

add_library(GTest INTERFACE IMPORTED)
  set_property(TARGET GTest PROPERTY INTERFACE_INCLUDE_DIRECTORIES
  ${DEPENDENCIES_DIR}/googletest/googletest/include
)
set_property(TARGET GTest PROPERTY INTERFACE_LINK_LIBRARIES
  gtest
  gtest_main
  Threads::Threads
)

set(CMAKE_BUILD_TYPE ${BUILD_TYPE})


cmake_minimum_required(VERSION 3.2 FATAL_ERROR)

if (NOT TARGET GTest)
  set(BUILD_TYPE ${CMAKE_BUILD_TYPE})

  set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build gtest as release build")
  set(BUILD_GMOCK OFF CACHE STRING "Don't build gmock")
  set(BUILD_GTEST ON CACHE STRING "Build gtest")

  find_package(Threads REQUIRED)

  add_subdirectory(${DEPENDENCIES_DIR}/googletest googletest)

  add_library(GTest INTERFACE IMPORTED)

  set_property(TARGET GTest PROPERTY INTERFACE_LINK_LIBRARIES
    gtest
    gtest_main
    Threads::Threads
  )

  if(XCODE)
    set_property(TARGET GTest PROPERTY INTERFACE_COMPILE_OPTIONS
      -isystem ${DEPENDENCIES_DIR}/googletest/googletest/include
    )
  else()
    set_property(TARGET GTest PROPERTY INTERFACE_INCLUDE_DIRECTORIES
      ${DEPENDENCIES_DIR}/googletest/googletest/include
    )
    set_property(TARGET GTest PROPERTY INTERFACE_SYSTEM_INCLUDE_DIRECTORIES
      ${DEPENDENCIES_DIR}/googletest/googletest/include
    )
  endif()


  set(CMAKE_BUILD_TYPE ${BUILD_TYPE})
endif()


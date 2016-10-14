cmake_minimum_required(VERSION 3.2 FATAL_ERROR)

add_library(Eigen INTERFACE IMPORTED)
if(XCODE)
  set_property(TARGET Eigen PROPERTY INTERFACE_COMPILE_OPTIONS
    -isystem
      $<BUILD_INTERFACE:${DEPENDENCIES_DIR}/eigen>
      $<INSTALL_INTERFACE:include/eigen>
  )
else()
  set_property(TARGET Eigen PROPERTY INTERFACE_INCLUDE_DIRECTORIES
      $<BUILD_INTERFACE:${DEPENDENCIES_DIR}/eigen>
      $<INSTALL_INTERFACE:include/eigen>
  )
  set_property(TARGET Eigen PROPERTY INTERFACE_SYSTEM_INCLUDE_DIRECTORIES
      $<BUILD_INTERFACE:${DEPENDENCIES_DIR}/eigen>
      $<INSTALL_INTERFACE:include/eigen>
  )
endif()


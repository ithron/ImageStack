cmake_minimum_required(VERSION 3.2 FATAL_ERROR)

if(NOT TARGET Eigen)

  add_library(Eigen INTERFACE)
  if(XCODE)
    set_property(TARGET Eigen PROPERTY INTERFACE_COMPILE_OPTIONS
      $<BUILD_INTERFACE:-isystem${DEPENDENCIES_DIR}/eigen>
    )
  else()
    set_property(TARGET Eigen PROPERTY INTERFACE_INCLUDE_DIRECTORIES
        $<BUILD_INTERFACE:${DEPENDENCIES_DIR}/eigen>
    )
    set_property(TARGET Eigen PROPERTY INTERFACE_SYSTEM_INCLUDE_DIRECTORIES
        $<BUILD_INTERFACE:${DEPENDENCIES_DIR}/eigen>
    )
  endif()

  install(DIRECTORY ${DEPENDENCIES_DIR}/eigen/Eigen
    DESTINATION include/ImageStack
  )
  install(TARGETS Eigen EXPORT ImageStackExport)

endif()

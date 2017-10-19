cmake_minimum_required(VERSION 3.2 FATAL_ERROR)

if(NOT TARGET GSL)
  add_library(GSL INTERFACE)

  option(GSL_THROW_ON_CONTRACT_VALIDATION "Sould GSL throw on contract validation?" YES)

  if (XCODE)
    set_property(TARGET GSL PROPERTY INTERFACE_COMPILE_OPTIONS
      -isystem$<BUILD_INTERFACE:${DEPENDENCIES_DIR}/GSL/include>$<INSTALL_INTERFACE:include/ImageStack/gsl>
    )
  else()
    set_property(TARGET GSL PROPERTY INTERFACE_INCLUDE_DIRECTORIES
      $<BUILD_INTERFACE:${DEPENDENCIES_DIR}/GSL/include>
      $<INSTALL_INTERFACE:include/ImageStack/gsl>
    )
    set_property(TARGET GSL PROPERTY INTERFACE_SYSTEM_INCLUDE_DIRECTORIES
      $<BUILD_INTERFACE:${DEPENDENCIES_DIR}/GSL/include>
      $<INSTALL_INTERFACE:include/ImageStack/include>
    )
  endif()

  if(GSL_THROW_ON_CONTRACT_VALIDATION)
    set_property(TARGET GSL PROPERTY INTERFACE_COMPILE_DEFINITIONS
      GSL_THROW_ON_CONTRACT_VIOLATION
    )
  endif()

  install(TARGETS GSL EXPORT ImageStackExport)
  install(DIRECTORY ${DEPENDENCIES_DIR}/GSL/include
    DESTINATION include/ImageStack
  )
endif()

find_package(MKL)

if(NOT TARGET MKL)
  if(MKL_FOUND)
    option(USE_MKL "Use MKL to speed up Eigen operations" YES)
  endif()

  if(MKL_FOUND AND USE_MKL)
    if (NOT HAS_MKL)
      set(HAS_MKL YES)

      message("dirs: ${MKL_LIBRARY_DIRS}")
      add_library(MKL INTERFACE IMPORTED)
      set_property(TARGET MKL PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${MKL_INCLUDE_DIRS})
      if(XCODE)
        set_property(TARGET MKL PROPERTY INTERFACE_COMPILE_OPTIONS
          -isystem${MKL_INCLUDE_DIRS}
        )
      else()
        set_property(TARGET MKL PROPERTY INTERFACE_SYSTEM_INCLUDE_DIRECTORIES ${MKL_INCLUDE_DIRS})
      endif()
      set_property(TARGET MKL PROPERTY INTERFACE_LINK_LIBRARIES ${MKL_LIBRARIES})
      set_property(TARGET MKL PROPERTY INTERFACE_COMPILE_OPTIONS "-L${MKL_LIBRARY_DIRS}")
      set_property(TARGET MKL PROPERTY INTERFACE_COMPILE_DEFINITIONS "EIGEN_USE_MKL_ALL")
    endif()
    set(MKL_TARGET MKL)
  else()
    set(MKL_TARGET "")
  endif()
endif()

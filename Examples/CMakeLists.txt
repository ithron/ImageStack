cmake_minimum_required(VERSION 3.2 FATAL_ERROR)

if (NOT TARGET VolViz::VolViz)
  set(BUILD_EXAMPLES OFF)
  add_subdirectory(${DEPENDENCIES_DIR}/VolViz VolViz)
  set(BUILD_EXAMPLES ON)
endif()

add_executable(ImageViewer imageViewer.cpp)
target_link_libraries(ImageViewer PRIVATE ImageStack VolViz::VolViz)


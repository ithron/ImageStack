cmake_minimum_required(VERSION 3.2 FATAL_ERROR)

add_subdirectory(${DEPENDENCIES_DIR}/VolViz VolViz)

add_executable(ImageViewer imageViewer.cpp)
target_link_libraries(ImageViewer PRIVATE ImageStack VolViz::VolViz)


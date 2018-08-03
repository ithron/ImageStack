cmake_minimum_required(VERSION 3.2 FATAL_ERROR)

hunter_add_package(VolViz)
find_package(VolViz CONFIG REQUIRED)

add_executable(ImageViewer imageViewer.cpp)
target_link_libraries(ImageViewer PRIVATE ImageStack VolViz::VolViz OpenMP)


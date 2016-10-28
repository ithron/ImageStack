#include <ImageStack/ImageStack.h>
#include <ImageStack/ImageStackLoaderBST.h>
#include <ImageStack/ResolutionDecorator.h>
#include <VolViz/VolViz.h>

#include <gsl.h>

#include <iostream>

using namespace ImageStack;
using namespace VolViz;
using namespace VolViz::literals;

using Img = ::ImageStack::ImageStack<float, HostStorage, ResolutionDecorator>;
using Loader = ::ImageStack::ImageStackLoaderBST<Img>;

int main(int argc, char **argv) {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " image" << std::endl;
    return EXIT_FAILURE;
  }

  try {
    Img const img((Loader(argv[1])));

    Visualizer viewer;

    AxisAlignedPlaneDescriptor plane;
    plane.axis = Axis::X;
    plane.color = Colors::White();
    plane.intercept = 0_mm;
    viewer.addGeometry("X-Plane", plane);

    plane.axis = Axis::Y;
    viewer.addGeometry("Y-Plane", plane);

    plane.axis = Axis::Z;
    viewer.addGeometry("Z-Plane", plane);

    VolumeDescriptor vol;
    vol.voxelSize[0] = img.resolution[0] * 1_mm;
    vol.voxelSize[1] = img.resolution[1] * 1_mm;
    vol.voxelSize[2] = img.resolution[2] * 1_mm;
    vol.size = img.size();

    auto const map = img.map();
    viewer.setVolume(vol, gsl::span<float const>(
                              map.data(), gsl::narrow<int>(map.linearSize())));

    viewer.start();
    viewer.renderOnUserInteraction();

  } catch (std::runtime_error const &e) {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

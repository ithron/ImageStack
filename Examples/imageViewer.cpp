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
  if (argc != 2 && argc != 4) {
    std::cerr << "Usage: " << argv[0] << " image [min max]" << std::endl;
    return EXIT_FAILURE;
  }

  Range<float> range{0.f, 0.f};

  if (argc == 4) {
    range.min = gsl::narrow<float>(atof(argv[2]));
    range.max = gsl::narrow<float>(atof(argv[3]));
  }

  try {
    Img const img((Loader(argv[1])));

    Visualizer viewer;

    viewer.showGrid = false;

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
    vol.type = VolumeType::GrayScale;
    vol.range = range;
    vol.interpolation = InterpolationType::Linear;

    auto const map = img.map();
    viewer.setVolume(vol, gsl::span<float const>(
                              map.data(), gsl::narrow<int>(map.linearSize())));

    Light light;
    light.ambientFactor = 1.0f;
    light.color = Colors::White();
    light.position = PositionH(1, 1, 1, 0);
    viewer.addLight(0, light);

    viewer.scale = 1_mm;

    viewer.start();
    viewer.renderOnUserInteraction();

  } catch (std::runtime_error const &e) {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

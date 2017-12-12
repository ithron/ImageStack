#include <ImageStack/GaussFilter.h>
#include <ImageStack/ImageStack.h>
#include <ImageStack/ImageStackLoaderBST.h>
#include <ImageStack/ResolutionDecorator.h>
#include <VolViz/VolViz.h>

#include <iostream>

using namespace ImageStack;
using namespace VolViz;
using namespace VolViz::literals;

using Img = ::ImageStack::ImageStack<float, HostStorage, ResolutionDecorator>;
using Loader = ::ImageStack::ImageStackLoaderBST<Img>;

int main(int argc, char **argv) {
  if (argc != 2 && argc != 4 && argc != 3 && argc != 5) {
    std::cerr << "Usage: " << argv[0] << " image [[min max] sigma]"
              << std::endl;
    return EXIT_FAILURE;
  }

  Range<float> range{0.f, 0.f};
  float sigma{0};

  if (argc == 4 || argc == 5) {
    range.min = narrow<float>(atof(argv[2]));
    range.max = narrow<float>(atof(argv[3]));
  }
  if (argc == 3) {
    sigma = narrow_cast<float>(atof(argv[2]));
  } else if (argc == 5) {
    sigma = narrow_cast<float>(atof(argv[4]));
  }

  try {
    Img const img = [argv, &sigma]() {
      Img const imgOrig((Loader(argv[1])));
      if (sigma == .0f) return imgOrig;
      Vector3f const res = resolution(imgOrig).template cast<float>();
      auto const filter = Filter::GaussFilter<float>{sigma * res};
      std::cout << "Using filter size " << filter.size().transpose()
                << std::endl;
      Img image = Filter::filter(imgOrig, filter);
      image.resolution = res.template cast<double>();
      return image;
    }();

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

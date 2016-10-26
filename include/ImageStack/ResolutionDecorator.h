#pragma once

#include "Types.h"

namespace ImageStack {

class ResolutionDecorator {
public:
  Eigen::Vector3d resolution{0, 0, 0};

protected:
  ResolutionDecorator() = default;
  template <class Loader> ResolutionDecorator(Loader &&loader) {
    resolution = loader.resolution();
  }
};

} // namespace ImageStack

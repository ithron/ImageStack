#pragma once

#include "ImageStack.h"
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

template <class IS,
          typename = std::enable_if_t<hasDecorator_v<IS, ResolutionDecorator>>>
auto resolution(IS const &is) {
  return is.resolution;
}

template <class IS,
          typename = std::enable_if_t<!hasDecorator_v<IS, ResolutionDecorator>>>
Eigen::Vector3d resolution(IS const &) {
  return Eigen::Vector3d::Ones();
}

} // namespace ImageStack

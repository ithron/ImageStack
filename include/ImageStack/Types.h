#pragma once

#include <Eigen/Core>
#include <gsl>

namespace ImageStack {

using Size = std::size_t;
using Size3 = Eigen::Matrix<std::size_t, 3, 1>;

using Index = std::size_t;
using Index3 = Eigen::Matrix<std::size_t, 3, 1>;

} // namespace ImageStack

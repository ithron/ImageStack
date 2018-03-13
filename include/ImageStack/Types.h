#pragma once

#include <Eigen/Core>
#include <gsl/gsl>

namespace ImageStack {

using namespace gsl;

using Size = std::size_t;
using Size3 = Eigen::Matrix<std::size_t, 3, 1>;

using Index = std::size_t;
using Index3 = Eigen::Matrix<std::size_t, 3, 1>;

using SIndex = long;
using SIndex3 = Eigen::Matrix<SIndex, 3, 1>;

struct ParallelTag {};

} // namespace ImageStack

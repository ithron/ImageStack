#pragma once

#include <Eigen/Core>

#include <type_traits>

namespace ImageStack {

template <class T>
constexpr bool isEigenMatrix_v =
    std::is_base_of<Eigen::MatrixBase<std::decay_t<T>>, std::decay_t<T>>::value;

} // namespace ImageStack


#pragma once

#include <Eigen/Core>

#include <type_traits>

namespace ImageStack {

/// @brief Test if a type is a Eigen matrix type
template <class T>
constexpr bool isEigenMatrix_v =
    std::is_base_of<Eigen::MatrixBase<std::decay_t<T>>, std::decay_t<T>>::value;

// Compile time tests
static_assert(!isEigenMatrix_v<int>, "int is not an Eigen matrix type");
static_assert(!isEigenMatrix_v<std::array<int, 3>>,
              "std::array is not an Eigen matrix type");
static_assert(isEigenMatrix_v<Eigen::Matrix3f>,
              "Eigen::Matrix3f is an Eigen matrix type");
static_assert(isEigenMatrix_v<Eigen::Vector2i>,
              "Eigen::Vector2i is an Eigen matrix type");
static_assert(isEigenMatrix_v<decltype(Eigen::Vector3i::Zero())>,
              "Eigen::Vector3i::Zero() is an Eigen matrix type");

} // namespace ImageStack

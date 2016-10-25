#pragma once

#include <Eigen/Core>

#include <type_traits>

namespace ImageStack {

/// @defgroup TypeTraits Type Traits
/// @{

/// @brief Type traits class to check if a given type is a Eigen matrix
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

/// @brief Checks if a template argument list contains a given type
template <class Type, class... Types>
struct ContainsType : public std::false_type {};

template <class Type, class Head, class... Tail>
struct ContainsType<Type, Head, Tail...>
    : public std::conditional_t<std::is_same<Type, Head>::value, std::true_type,
                                ContainsType<Type, Tail...>> {};

template <class Type> struct ContainsType<Type> : public std::false_type {};

/// @brief Alias for ContainsType<>::type
template <class T> using ContainsType_t = typename ContainsType<T>::type;

/// @}

} // namespace ImageStack

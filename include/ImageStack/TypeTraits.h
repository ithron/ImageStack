#pragma once

#include <Eigen/Core>

#include <array>
#include <type_traits>

namespace ImageStack {

/// @defgroup TypeTraits Type Traits
/// @{

template <class T>
struct isEigenMatrix
    : public std::integral_constant<
          bool, std::is_base_of<Eigen::MatrixBase<std::decay_t<T>>,
                                std::decay_t<T>>::value> {};

template <class Derived>
struct isEigenMatrix<Eigen::MatrixBase<Derived>> : public std::true_type {};
template <class Derived>
struct isEigenMatrix<Eigen::MatrixBase<Derived> &> : public std::true_type {};
template <class Derived>
struct isEigenMatrix<Eigen::MatrixBase<Derived> const &>
    : public std::true_type {};
template <class Derived>
struct isEigenMatrix<Eigen::MatrixBase<Derived> &&> : public std::true_type {};

/// @brief Type traits class to check if a given type is a Eigen matrix
template <class T> constexpr bool isEigenMatrix_v = isEigenMatrix<T>::value;

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
static_assert(isEigenMatrix_v<Eigen::MatrixBase<Eigen::Vector3d>>,
              "Eigen::Vector3d is an Eigen matrix type");

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

/// @brief Type trait to check if a given type is model of Container
/// @ingroup TypeTraits
template <class T> class IsContainer {

  template <
      class S,
      typename = std::enable_if_t<
          std::is_convertible<typename S::size_type,
                              decltype(std::declval<S>().size())>::value &&
          std::is_convertible<typename S::value_type,
                              std::remove_reference_t<decltype(
                                  *std::declval<S>().begin())>>::value>>
  static constexpr std::true_type check(S *) {
    return std::true_type{};
  }

  template <class> static constexpr std::false_type check(...) {
    return std::false_type{};
  }

  using type = decltype(check<T>(0));

public:
  static constexpr bool value = type::value;
};

/// @brief Alias for `IsContainer<T>::value`
/// @ingroup TypeTraits
template <class T> constexpr bool isContainer_v = IsContainer<T>::value;

/// @}

} // namespace ImageStack

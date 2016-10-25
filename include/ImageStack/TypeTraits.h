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

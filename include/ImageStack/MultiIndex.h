/// @defgroup MultiIndexConcept Multi Index Concept
/// @par Description
/// A multi index is a non-empty tuple of indices. The length of the tuple is
/// called it's dimension and must not be 0. The dimension of a multi index
/// must be known at compile time.
///
/// @par Notations
/// Let @c I be an model of \ref MultiIndexConcept, and let @c i be an instance
/// of @c I. Let @c j be an unsigned integer number.
///
/// @par Valid Expressions
/// - <b>i[j]</b> returns the index of dimension @c j and must be
///   trivially convertable to @c std::size_t
/// - <b>dims_v\<I\> > 0</b>
///
/// Models of \ref MultiIndexConcept :
/// - C style arrays
/// - @c std::array
/// - @c gsl::span
/// - Eigen fixes size row or column vectors
#pragma once

#include "TypeTraits.h"

#include <Eigen/Core>

#include <array>
#include <tuple>
#include <type_traits>
#include <utility>

namespace ImageStack {

/// @defgroup MultiIndexConceptTypeTraits Type Traits
/// @{
/// @ingroup MultiIndexConcept

/// Integral constant represening the number of dimenions of @c T.
///
/// @note Should be specialized for models of \ref MultiIndexConcept that do not
/// have a std::tuple_size<> specialization.
template <class T, typename = void>
struct Dims : public std::integral_constant<std::size_t, 0> {};

// Overload for std::array types
template <class T, std::size_t N>
struct Dims<std::array<T, N>> : public std::integral_constant<std::size_t, N> {
};

// Dims<> overload for C arrays
template <class T, std::size_t N>
struct Dims<T[N]> : public std::integral_constant<std::size_t, N> {};

// Dims<> overload for Eigen vectors
template <class Derived>
struct Dims<Derived, std::enable_if_t<isEigenMatrix_v<Derived> &&
                                      (Derived::RowsAtCompileTime == 1 ||
                                       Derived::ColsAtCompileTime == 1)>>
    : public std::integral_constant<std::size_t,
                                    std::max(Derived::RowsAtCompileTime,
                                             Derived::ColsAtCompileTime)> {};

/// Alias for Dims::value
template <class T> constexpr std::size_t dims_v = Dims<T>::value;

/// Constexpr function for dim_v
template <class I> constexpr std::size_t dims(I const &) noexcept {
  return dims_v<I>;
}

/// Tests if @c T is a model of \ref MultiIndexConcept.
template <class T, typename = void>
struct IsModelOfMultiIndex : public std::false_type {};

template <class T>
struct IsModelOfMultiIndex<
    T, std::enable_if_t<std::is_convertible<decltype(std::declval<T>()[0]),
                                            std::size_t>::value &&
                        (dims_v<T>> 0)>> : public std::true_type {};

/// Alias for IsModelOfMultiIndex<T>::value
template <class T>
constexpr bool isModelOfMultiIndex_v = IsModelOfMultiIndex<T>::value;

/// @}

/// @defgroup MultiIndexConceptUtilities Utilities
/// @{
/// @ingroup MultiIndexConcept

/// @brief Converts the given multi index to a linear index using the given
/// order
///
/// The order of the dimensions that are used for linear index computation can
/// be specified by the template parameter @c Order. E.g. to compute a linear
/// index of a 2d multi index (row, column) in column-major order, the oder
/// would be 0, 1. For row-major order, the order would be 1, 0.
///
/// Test cases are in \ref testMultiIndex.cpp
/// @tparam I model of \ref MultiIndexConcept
/// @tparam S model of \ref MultiIndexConcept
/// @tparam Order order of dimensions
/// @param s tuple containing the size of each dimension
/// @return linear representation of the multi index i
template <
    class I, class S, std::size_t... Order,
    typename = std::enable_if_t<isModelOfMultiIndex_v<I> &&
                                (isModelOfMultiIndex_v<S> || (dims_v<I> == 1))>>
constexpr std::size_t
toLinearReorder(I const &i, S const &s,
                std::index_sequence<Order...> =
                    std::index_sequence<Order...>{}) noexcept(noexcept(i[0]) &&
                                                              noexcept(s[0])) {
  using std::size_t;
  static_assert(dims_v<I> - 1 <= dims_v<S>, "Number of dimensions of I - 1 "
                                            "must not exceed number of "
                                            "dimensions of S");
  static_assert(
      dims_v<I> == sizeof...(Order),
      "Number of ordering elements does not match number of dimensions of I");

  std::array<size_t, sizeof...(Order)> constexpr order{{Order...}};

  // The linear index to be computed. Initialized with the index of the first
  // (possible reordered) dimension.
  size_t linIdx = static_cast<size_t>(i[order[0]]);

  // In the computation of the linear index, the product of all previous
  // dimensions is required. This is stored here
  size_t sizeOfPrevDims = dims_v<I>> 1 ? static_cast<size_t>(s[order[0]]) : 0;

  for (size_t d = 1; d < dims_v<I>; ++d) {
    linIdx += static_cast<size_t>(i[order[d]]) * sizeOfPrevDims;
    sizeOfPrevDims *= static_cast<size_t>(s[order[d]]);
  }

  return linIdx;
}

/// @brief Converts the given multi index to a linear index
/// Test cases are in \ref testMultiIndex.cpp
/// @tparam I model of \ref MultiIndexConcept
/// @tparam S model of \ref MultiIndexConcept
/// @param i multi index to convert
/// @param s tuple containing the size of each dimension
/// @return linear representation of the multi index i
template <class I, class S, typename = std::enable_if_t<
                                isModelOfMultiIndex_v<I> &&
                                (isModelOfMultiIndex_v<S> | (dims_v<I> == 1))>>
constexpr std::size_t
toLinear(I const &i, S const &s) noexcept(noexcept(i[0]) && noexcept(s[0])) {
  return toLinearReorder(i, s, std::make_index_sequence<dims_v<I>>());
}

/// @brief Converts the given multi index to a linear index, overload for 1d
/// indices
///
/// This function is an overload of toLinear for linear indices
///
/// Test cases are in \ref testMultiIndex.cpp
/// @tparam S model of \ref MultiIndexConcept
/// @param i index to convert
/// @return linear representation of the multi index i
template <class S, typename = std::enable_if_t<isModelOfMultiIndex_v<S>>>
constexpr std::size_t toLinear(std::size_t i, S const &) noexcept {
  return i;
}

/// @brief Computed the sum of all indices of a multi index
/// Test cases are in \ref testMultiIndex.cpp
/// @tparam I model of \ref MultiIndexConcept
/// @param i multi index
/// @return sum of all indices in @c i
template <class I, typename = std::enable_if_t<isModelOfMultiIndex_v<I>>>
constexpr auto indexSum(I const &i) noexcept(noexcept(i[0])) {
  auto sum = i[0];
  for (std::size_t j = 1; j < dims_v<I>; ++j) sum += i[j];
  return sum;
}

/// @brief Computed the product of all indices of a multi index
/// Test cases are in \ref testMultiIndex.cpp
/// @tparam I model of \ref MultiIndexConcept
/// @param i multi index
/// @return sum of all indices in @c i
template <class I, typename = std::enable_if_t<isModelOfMultiIndex_v<I>>>
constexpr auto indexProduct(I const &i) noexcept(noexcept(i[0])) {
  auto prod = i[0];
  for (std::size_t j = 1; j < dims_v<I>; ++j) prod *= i[j];
  return prod;
}

namespace detail {

template <class I, std::size_t... Dims>
constexpr auto
subindexHelper(I const &i,
               std::index_sequence<Dims...>) noexcept(noexcept(i[0])) {
  auto constexpr N = sizeof...(Dims);
  using Index = std::decay_t<decltype(i[0])>;
  return std::array<Index, N>{{i[Dims]...}};
}

} // namespace detail

/// @brief Returns a subset of a multi index
/// Test cases are in \ref testMultiIndex.cpp
/// @tparam N size of the subset
/// @tparam Dims list of dimensions to include in the subset, if none are
/// specified, the first N dimensions are choosen
/// @tparam I model ofer \ref MultiIndexConcept
/// @param i multi index
/// @return a multi index of dimension @c N, containing the selected dimensions
template <std::size_t N, std::size_t... Dims, class I>
constexpr auto subindex(I const &i) noexcept(noexcept(i[0])) {
  auto constexpr M = sizeof...(Dims);
  static_assert(N == M || M == 0, "Number of selected dimensions must match N");
  return detail::subindexHelper(
      i, std::conditional_t<M == 0, decltype(std::make_index_sequence<N>()),
                            std::index_sequence<Dims...>>{});
}

/// @}

} // namespace ImageStack

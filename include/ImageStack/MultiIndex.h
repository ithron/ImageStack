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
/// - <b>dims_v<I> > 0</b>
///
/// Models of \ref MultiIndexConcept:
/// - C style arrays
/// - @c std::array
/// - @c gsl::span
/// - Eigen fixes size row or column vectors
#pragma once

#include <ImageStack/Debug.h>
#include <ImageStack/TypeTraits.h>

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
template <class T, typename = void> struct Dims;

// Overload for every type with an std::tuple_size specialization
template <class T>
struct Dims<T, std::enable_if_t<std::is_convertible<
                   decltype(std::tuple_size<T>::value), std::size_t>::value>>
    : public std::tuple_size<T> {};

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
template <class T>
struct isModelOfMultiIndex
    : public std::integral_constant<
          bool, std::is_convertible<decltype(std::declval<T>()[0]),
                                    std::size_t>::value &&
                    (dims_v<T>> 0)> {};

/// Alias for isModelOfMultiIndex<T>::value
template <class T>
constexpr bool isModelOfMultiIndex_v = isModelOfMultiIndex<T>::value;

/// @}

/// @defgroup MultiIndexConceptUtilities Utilities
/// @{
/// @ingroup MultiIndexConcept

/// Converts the given multi index to a linear index using the given order
///
/// The order of the dimensions that are used for linear index computation can
/// be specified by the template parameter @c Order. E.g. to compute a linear
/// index of a 2d multi index (row, column) in column-major order, the oder
/// would be 0, 1. For row-major order, the order would be 1, 0.
///
/// @tparam I model of \ref MultiIndexConcept
/// @tparam S model of \ref MultiIndexConcept
/// @tparam Order order of dimensions
/// @param s tuple containing the size of each dimension
/// @return linear representation of the multi index i
template <class I, class S, std::size_t... Order,
          typename = std::enable_if_t<isModelOfMultiIndex_v<I> &&
                                      isModelOfMultiIndex_v<S>>>
constexpr std::size_t
toLinearReorder(I const &i, S const &s,
                std::index_sequence<Order...> =
                    std::index_sequence<Order...>{}) noexcept(noexcept(i[0]) &&
                                                              noexcept(s[0])) {
  using std::size_t;
  static_assert(dims_v<I> == dims_v<S>,
                "Number of dimensions of I and S do not match");
  static_assert(
      dims_v<I> == sizeof...(Order),
      "Number of ordering elements does not match number of dimensions of I");

  std::array<size_t, sizeof...(Order)> constexpr order{{Order...}};

  size_t linIdx = static_cast<size_t>(i[order[0]]);
  for (size_t d = 1; d < dims_v<I>; ++d)
    linIdx +=
        static_cast<size_t>(i[order[d]]) * static_cast<size_t>(s[order[d - 1]]);

  return linIdx;
}

/// Converts the given multi index to a linear index
/// @tparam I model of \ref MultiIndexConcept
/// @tparam S model of \ref MultiIndexConcept
/// @param i multi index to convert
/// @param s tuple containing the size of each dimension
/// @return linear representation of the multi index i
template <class I, class S,
          typename = std::enable_if_t<isModelOfMultiIndex_v<I> &&
                                      isModelOfMultiIndex_v<S>>>
constexpr std::size_t
toLinear(I const &i, S const &s) noexcept(noexcept(i[0]) && noexcept(s[0])) {
  return toLinearReorder(i, s, std::make_index_sequence<dims_v<I>>());
}

static_assert(toLinear(std::array<std::size_t, 2>{{1, 2}},
                       std::array<std::size_t, 2>{{10, 20}}) == 21,
              "Error");
static_assert(toLinearReorder(std::array<std::size_t, 2>{{1, 2}},
                              std::array<std::size_t, 2>{{10, 20}},
                              std::index_sequence<1, 0>{}) == 22,
              "Error");

/// @}

} // namespace ImageStack


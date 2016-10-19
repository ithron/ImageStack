#pragma once

#include <ImageStack/MultiIndex.h>
#include <ImageStack/Types.h>

namespace ImageStack {

/// @brief Base class for multi dimensional mapped memory
///
/// @tparam T type of stored elements
/// @tparam dims number of dimensiosn of the mapping, must be > 0
template <class T, Size dims = 1> class MappedMemoryBase {
  using Memory = gsl::span<T>;

public:
  static_assert(dims > 0, "dims must not be 0");

  using value_type = T;
  using reference = T &;
  using const_reference = T const &;
  using iterator = typename Memory::iterator;
  using const_iterator = typename Memory::const_iterator;
  using difference_type = typename Memory::difference_type;
  using size_type = typename Memory::size_type;

  /// @brief Const access the mapped memory using a multi index
  /// @tparam Idx model of \ref MultiIndexConcept
  /// @param i multi index
  /// @return const reference to element at @c i
  template <class Idx,
            typename = std::enable_if_t<isModelOfMultiIndex_v<Idx> ||
                                        std::is_convertible<Idx, Size>::value>>
  constexpr T const &operator[](Idx const &i) const {
    return memory_[toLinear(i, dims_)];
  }

  /// @brief Access the mapped memory using a multi index
  /// @tparam Idx model of \ref MultiIndexConcept
  /// @param i multi index
  /// @return reference to element at @c i
  template <class Idx,
            typename = std::enable_if_t<isModelOfMultiIndex_v<Idx> ||
                                        std::is_convertible<Idx, Size>::value>>
  T &operator[](Idx const &i) {
    return memory_[toLinear(i, dims_)];
  }

  /// @brief Returns the linear size of the mapped memory region
  ///
  /// The linear size is the product of the sizes of all dimensions
  constexpr Size linearSize() const noexcept { return memory_.size(); }

  /// @brief Returns the multi dimensional size of the mapped memory region
  ///
  /// @return object containing the size of each dimensions. The returned object
  /// in an instance of a model of \ref MultiIndexConcept.
  ///
  /// @note This function returns a newly created @c std::array object, so
  ///   better store the result if it is used frequently.
  constexpr auto size() const noexcept {
    auto const prod = indexProduct(dims_);
    Expects(linearSize() % prod == 0);

    std::array<Size, dims> res;
    for (int i = 0; i < dims - 1; ++i) res[i] = dims_[i];
    res[dims - 1] = linearSize() / prod;

    return res;
  }

  /// @brief Returns a const_iterator pointing to the beginning of the mapped
  /// memory
  constexpr const_iterator begin() const noexcept { return memory_.begin(); }
  /// @brief Returns a const_iterator pointing to the beginning of the mapped
  /// memory
  constexpr const_iterator cbegin() const noexcept { return memory_.cbegin(); }
  /// @brief Returns a const_iterator pointing to one element after the end of
  /// the memory region
  constexpr const_iterator end() const noexcept { return memory_.end(); }
  /// @brief Returns a const_iterator pointing to one element after the end of
  /// the memory region
  constexpr const_iterator cend() const noexcept { return memory_.cend(); }

  /// @brief Returns a iterator pointing to the beginning of the mapped memory
  iterator begin() noexcept { return memory_.begin(); }
  /// @brief Returns a iterator pointing to one element after the end of the
  /// memory region
  iterator end() noexcept { return memory_.end(); }

protected:
  /// @brief Protected constructor
  /// @tparam S model of \ref MultiIndexConcept
  /// @param memory @c gsl::span<T> pointing to the mapped memory
  /// @param size size of the first <b>dims - 1</b> dimensions
  template <class S, typename = std::enable_if_t<isModelOfMultiIndex_v<S>>>
  constexpr MappedMemoryBase(gsl::span<T> memory,
                             S const &size) noexcept(noexcept(size[0]))
      : memory_(memory), dims_(size) {}

private:
  template <class S, std::size_t... I>
  constexpr MappedMemoryBase(
      gsl::span<T> memory, S const &size,
      std::index_sequence<I...>) noexcept(noexcept(size[0]))
      : MappedMemoryBase(memory, size, std::make_index_sequence<dims - 1>()) {}

  /// @brief Store the mapped region
  gsl::span<T> memory_;
  /// @brief Store sizes of the the first <b>dims - 1</b> dimensions
  std::array<Size, dims - 1> dims_;
};

/// @brief Class represensing multi dimensional host memory
///
/// Instances of this class represent memory regions directly accessible by the
/// CPU.
/// @tparam T type of elements stored in the memory region
/// @tparam dims number of dimensions of the memory region
template <class T, Size dims = 1>
class MappedHostMemory : public MappedMemoryBase<T, dims> {
  using Base = MappedMemoryBase<T>;

public:
  /// @brief Create a host memory mapping by specifying its start and size
  /// @tparam S model of \ref MultiIndexConcept
  /// @param startPtr pointer to the start of the region, must not be null_ptr
  /// @param size size of each dimension of the memory region
  template <class S, typename = std::enable_if_t<isModelOfMultiIndex_v<S>>>
  constexpr MappedHostMemory(
      gsl::not_null<T *> startPtr,
      S const &size) noexcept(noexcept(indexProduct(size)) &&
                              noexcept(MappedMemoryBase(
                                  std::declval < gsl::span<T>(), size)))
      : Base(gsl::as_span(startPtr, indexProduct(size)), size) {}
};

} // namespace ImageStack

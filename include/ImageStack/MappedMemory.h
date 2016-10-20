#pragma once

#include <ImageStack/MultiIndex.h>
#include <ImageStack/Types.h>

namespace ImageStack {

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wpadded"
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
  using difference_type = std::ptrdiff_t;
  using size_type = std::size_t;

  /// @brief Const access the mapped memory using a multi index
  /// @tparam Idx model of \ref MultiIndexConcept
  /// @param i multi index
  /// @return const reference to element at @c i
  template <class Idx,
            typename = std::enable_if_t<isModelOfMultiIndex_v<Idx> ||
                                        std::is_convertible<Idx, Size>::value>>
  inline T const &operator[](Idx const &i) const {
    // Bounds checking is done in gsl::span
    return memory_[toLinear(i, size_)];
  }

  /// @brief Access the mapped memory using a multi index
  /// @tparam Idx model of \ref MultiIndexConcept
  /// @param i multi index
  /// @return reference to element at @c i
  template <class Idx,
            typename = std::enable_if_t<isModelOfMultiIndex_v<Idx> ||
                                        std::is_convertible<Idx, Size>::value>>
  inline T &operator[](Idx const &i) {
    // Bounds checking is done in gsl::span
    return memory_[toLinear(i, size_)];
  }

  /// @brief Returns the linear size of the mapped memory region
  ///
  /// The linear size is the product of the sizes of all dimensions
  inline size_type linearSize() const noexcept {
    return static_cast<size_type>(memory_.size());
  }

  /// @brief Returns the multi dimensional size of the mapped memory region
  ///
  /// @return object containing the size of each dimensions. The returned object
  /// in an instance of a model of \ref MultiIndexConcept.
  ///
  /// @note This function returns a newly created @c std::array object, so
  ///   better store the result if it is used frequently.
  inline auto size() const noexcept {
    return size(std::integral_constant<bool, (dims > 1)>{});
  }

  /// @brief Returns a const_iterator pointing to the beginning of the mapped
  /// memory
  inline const_iterator begin() const noexcept { return memory_.cbegin(); }
  /// @brief Returns a const_iterator pointing to the beginning of the mapped
  /// memory
  inline const_iterator cbegin() const noexcept { return memory_.cbegin(); }
  /// @brief Returns a const_iterator pointing to one element after the end of
  /// the memory region
  inline const_iterator end() const noexcept { return memory_.cend(); }
  /// @brief Returns a const_iterator pointing to one element after the end of
  /// the memory region
  inline const_iterator cend() const noexcept { return memory_.cend(); }

  /// @brief Returns a iterator pointing to the beginning of the mapped memory
  inline iterator begin() noexcept { return memory_.begin(); }
  /// @brief Returns a iterator pointing to one element after the end of the
  /// memory region
  inline iterator end() noexcept { return memory_.end(); }

protected:
  /// @brief Protected constructor
  /// @tparam S model of \ref MultiIndexConcept
  /// @param memory @c gsl::span<T> pointing to the mapped memory
  /// @param size size of the first <b>dims - 1</b> dimensions
  template <class S, typename = std::enable_if_t<isModelOfMultiIndex_v<S>>>
  inline MappedMemoryBase(gsl::not_null<T *> memory,
                          S const &size) noexcept(noexcept(size[0]))
      : MappedMemoryBase(memory, size,
                         std::integral_constant<bool, (dims > 1)>{}) {}

private:
  template <class S>
  inline MappedMemoryBase(gsl::not_null<T *> memory, S const &size,
                          std::false_type) noexcept(noexcept(size[0]))
      : memory_(memory, gsl::narrow_cast<std::ptrdiff_t>(size[0])) {}

  template <class S>
  inline MappedMemoryBase(gsl::not_null<T *> memory, S const &size,
                          std::true_type) noexcept(noexcept(size[0]))
      : MappedMemoryBase(memory, size, std::make_index_sequence<dims - 1>{}) {}

  template <class S, std::size_t... I>
  inline MappedMemoryBase(gsl::not_null<T *> memory, S const &size,
                          std::index_sequence<I...>) noexcept(noexcept(size[0]))
      : memory_(memory, static_cast<std::ptrdiff_t>(
                            indexProduct(subindex<dims - 1, I...>(size)) *
                            size[dims - 1])),
        size_({{size[I]...}}) {}

  inline auto size(std::true_type) const noexcept {
    auto const prod = indexProduct(size_);
    Expects(linearSize() == 0 || linearSize() % prod == 0);

    std::array<size_type, dims> res;
    for (size_type i = 0; i < dims - 1; ++i) res[i] = size_[i];
    res[dims - 1] = linearSize() > 0 ? linearSize() / prod : 0;

    return res;
  }

  inline auto size(std::false_type) const noexcept {
    return std::array<Size, 1>{{linearSize()}};
  }

  /// @brief Store the mapped region
  gsl::span<T> memory_;
  /// @brief Store sizes of the the first <b>dims - 1</b> dimensions
  std::array<Size, dims - 1> size_;
};

/// @brief Class represensing multi dimensional host memory
///
/// Instances of this class represent memory regions directly accessible by the
/// CPU.
///
/// Test cases are in \ref testMappedMemory.cpp
///
/// @tparam T type of elements stored in the memory region
/// @tparam dims number of dimensions of the memory region
template <class T, Size dims = 1>
class MappedHostMemory : public MappedMemoryBase<T, dims> {
  using Base = MappedMemoryBase<T, dims>;

public:
  /// @brief Create a host memory mapping by specifying its start and size
  /// @tparam S model of \ref MultiIndexConcept
  /// @param startPtr pointer to the start of the region, must not be null_ptr
  /// @param size size of each dimension of the memory region
  template <class S, typename = std::enable_if_t<isModelOfMultiIndex_v<S>>>
  inline MappedHostMemory(gsl::not_null<T *> startPtr,
                          S const &size) noexcept(noexcept(indexProduct(size)))
      : Base(startPtr, size) {}
};

#pragma clang diagnostic pop

} // namespace ImageStack

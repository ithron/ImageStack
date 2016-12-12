#pragma once

#include <ImageStack/MappedMemory.h>
#include <ImageStack/Types.h>

#include <vector>

namespace ImageStack {

/// @brief Class representing a 3D data storage in host memory
///
/// Unit tests are in \ref testHostStorage.cpp
/// @tparam T type of stored elements
template <class T> class HostStorage {
public:
  using ValueType = T;
  using Pointer = T *;

  /// @brief Create host storage object and allocate memory
  /// @tparam Size model of \ref MultiIndexConcept with at least 3 dimensions
  /// @param size size of storage to be allocated
  /// @note if Size has more than 3 dimensions, only the first 3 dimensions are
  /// used
  template <class Size, typename = std::enable_if_t<
                            isModelOfMultiIndex_v<Size> && (dims_v<Size> >= 3)>>
  inline explicit HostStorage(Size size)
      : size_(size[0], size[1], size[2]), storage_(indexProduct(size)) {}

  /// @brief Create host storage and initialize allocated memory with given
  /// value
  /// @tparam Size model of \ref MultiIndexConcept with at least 3 dimensions
  /// @param size size of storage to be allocated
  /// @param init value to initialize the memory with
  /// @note if Size has more than 3 dimensions, only the first 3 dimensions are
  /// used
  template <class Size, typename = std::enable_if_t<
                            isModelOfMultiIndex_v<Size> && (dims_v<Size> >= 3)>>
  inline HostStorage(Size size, T const &init)
      : size_(size[0], size[1], size[2]), storage_(indexProduct(size), init) {}

  /// @brief Create host storage and initialize allocated memory with given
  /// content
  /// @tparam Size model of \ref MultiIndexConcept with at least 3 dimensions
  /// @tparam Container model of ContiguousContainer
  /// @param size size of storage to be allocated
  /// @param init container containing the content to initialize the storage
  /// with
  /// @note if Size has more than 3 dimensions, only the first 3 dimensions are
  /// used
  template <
      class Size, class Container,
      // SFINAE check if Container is a model of ContiguousContainer.
      // Use .data() as a indicator that Container is.
      typename = std::enable_if_t<
          std::is_convertible<typename Container::pointer, Pointer>::value &&
          std::is_convertible<
              typename Container::pointer,
              decltype(std::declval<Container>().data())>::value &&
          std::is_convertible<
              typename Container::size_type,
              decltype(std::declval<Container>().size())>::value &&
          // Check if Size is a model of MultiIndexConcept with dims >= 3
          isModelOfMultiIndex_v<Size> && (dims_v<Size> >= 3)>>
  inline explicit HostStorage(Size size, Container const &init)
      : size_(size[0], size[1], size[2]), storage_(init.cbegin(), init.cend()) {
    Expects(indexProduct(size) == init.size());
  }

  /// @brief Returns the size of the storage
  /// @return an instance of a model of \ref MultiIndexConcept represening the
  /// size in each dimension
  inline auto size() const noexcept { return size_; }

  /// @brief Returns the linear size of the storage, i.e. the product of the
  /// size of each dimension
  /// @return linear size
  inline Size linearSize() const noexcept { return indexProduct(size_); }

  /// @brief Maps to storage to host memory and returs a memory mapping
  /// object
  /// @pre The storage object must not be empty
  /// @return MappedHostMemory object representing the mapping
  inline auto map() noexcept {
    Expects(!empty());
    return MappedHostMemory<T, 3>(storage_.data(), size_);
  }

  /// @brief Maps to storage to host memory and returs a const memory mapping
  /// object
  /// @return const MappedHostMemory object representing the mapping
  inline auto map() const noexcept {
    return MappedHostMemory<T const, 3>(storage_.data(), size_);
  }

  /// @brief Returns true if the the storage is empty, i.e. no memory is
  /// allocated
  /// @return true if empty
  inline bool empty() const noexcept { return linearSize() == 0; }

private:
  Size3 size_;
  std::vector<T> storage_;
};

template <template <class> class T>
struct IsHostStorage : public std::false_type {};

template <> struct IsHostStorage<HostStorage> : public std::true_type {};

template <template <class> class T>
constexpr bool isHostStorage_v = IsHostStorage<T>::value;

} // namespace ImageStack

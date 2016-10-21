#pragma once

#include <ImageStack/MappedMemory.h>
#include <ImageStack/Types.h>

#include <vector>

namespace ImageStack {

/// @brief Class representing a 3D data storage in host memory
/// @tparam T type of stored elements
template <class T> class HostStorage {
protected:
  /// @brief Create host storage object and allocate memory
  /// @param size size of storage to be allocated
  inline HostStorage(Size3 size) : size_(size) { storage_.reserve(size); }

  /// @brief Create host storage and initialize allocated memory with given
  /// value
  /// @param size size of storage to be allocated
  /// @param init value to initialize the memory with
  inline HostStorage(Size3 size, T const &init) : size_(size), storage_(size, init) {}

  /// @brief Maps to storage to host memory and returs a memeory mapping object
  /// 

private:
  Size3 size_;
  std::vector<T> storage_;
};

} // namespace ImageStack

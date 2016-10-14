#pragma once

#include <ImageStack/Types.h>

namespace ImageStack {

template <class T, Size dims = 1> class MappedMemoryBase {
public:
  static_assert(dims > 0, "dims mustnot be 0");

  template<class Idx>
  constexpr T const &operator[](Idx const &i) const {
    Size linIndex{i[0]};

    for (Size dim = dims - 1; dim > 0; --dim)
      linIndex += dims_[dim - 1] * i[dim];

    return memory_[linIndex];
  }

  constexpr T &operator[](Index i) { return memory_[i]; }

  constexpr Size size() const noexcept { return memory_.size(); }

protected:
  constexpr MappedMemoryBase(gsl::span<T> memory) noexcept : memory_(memory) {}

private:
  gsl::span<T> memory_;
  std::array<Size, dims - 1> dims_;
};

template <class T> class MappedHostMemory : public MappedMemoryBase<T> {
public:
  constexpr MappedHostMemory(gsl::not_null<T *> startPtr, Size size) noexcept
      : MappedMemoryBase(gsl::as_span(startPtr, size)) {}
};

} // namespace ImageStack

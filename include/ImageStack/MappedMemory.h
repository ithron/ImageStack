#pragma once

#include <ImageStack/MultiIndex.h>
#include <ImageStack/Types.h>

namespace ImageStack {

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

  template <class Idx,
            typename = std::enable_if_t<isModelOfMultiIndex_v<Idx> ||
                                        std::is_convertible<Idx, Size>::value>>
  constexpr T const &operator[](Idx const &i) const {
    return memory_[toLinear(i, dims_)];
  }

  template <class Idx,
            typename = std::enable_if_t<isModelOfMultiIndex_v<Idx> ||
                                        std::is_convertible<Idx, Size>::value>>
  T &operator[](Idx const &i) {
    return memory_[toLinear(i, dims_)];
  }

  constexpr Size linearSize() const noexcept { return memory_.size(); }

  constexpr auto size() const noexcept {
    auto const prod = indexProduct(dims_);
    Expects(linearSize() % prod == 0);

    std::array<Size, dims> res;
    for (int i = 0; i < dims - 1; ++i) res[i] = dims_[i];
    res[dims - 1] = linearSize() / prod;

    return res;
  }

  constexpr const_iterator begin() const noexcept { return memory_.begin(); }
  constexpr const_iterator cbegin() const noexcept { return memory_.cbegin(); }
  constexpr const_iterator end() const noexcept { return memory_.end(); }
  constexpr const_iterator cend() const noexcept { return memory_.cend(); }

  iterator begin() noexcept { return memory_.begin(); }
  iterator end() noexcept { return memory_.end(); }

protected:
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

  gsl::span<T> memory_;
  std::array<Size, dims - 1> dims_;
};

template <class T> class MappedHostMemory : public MappedMemoryBase<T> {
  using Base = MappedMemoryBase<T>;

public:
  template <class S, typename = std::enable_if_t<isModelOfMultiIndex_v<S>>>
  constexpr MappedHostMemory(gsl::not_null<T *> startPtr,
                             S const &size) noexcept
      : Base(gsl::as_span(startPtr, indexProduct(size)), size) {}
};

} // namespace ImageStack

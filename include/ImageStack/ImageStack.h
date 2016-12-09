#pragma once

#include "HostStorage.h"
#include "MultiIndex.h"
#include "Types.h"

#include <set>
#include <vector>

namespace ImageStack {

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wpadded"
/// @brief class representing a 3D image that can be represented as a stack of
/// 2D images
///
/// Test cases can be found in \ref testImageStack.cpp
/// @tparam T Pixel value type
/// @tparam Storage_ Storage type, must be a template class, taking T as its
/// parameter
/// @tparam Decorators List of implemented decorators
template <class T, template <class> class Storage_ = HostStorage,
          class... Decorators>
class ImageStack : public Decorators... {
  using Self = ImageStack<T, Storage_, Decorators...>;

public:
  using Storage = Storage_<T>;
  using StorageType = T;

  /// @brief Create an empty image stack
  inline ImageStack() noexcept : storage_(Size3::Zero()) {}

  /// @brief Loads an image stack using the given loader
  template <class Loader>
  explicit ImageStack(Loader &&loader)
      : Decorators(std::forward<Loader>(loader))..., storage_(loader.size()) {

    auto const size = loader.size();
    Expects(indexProduct(size) > 0);

    Storage store(size);
    loader.template readData<StorageType>(store.map().begin());

    storage_ = std::move(store);
  }

  /// @brief Cast constructor
  template <class ST, template <class> class S, class... Decs,
            typename = typename std::enable_if_t<
                std::is_convertible<ST, StorageType>::value>>
  ImageStack(ImageStack<ST, S, Decs...> const &stack) {

    Storage store(stack.size());
    auto const srcMap = stack.storage_.map();
    auto destMap = store.map();
    std::transform(srcMap.begin(), srcMap.end(), destMap.begin(),
                   [](auto v) { return static_cast<StorageType>(v); });

    storage_ = std::move(store);
  }

  /// @brief Returns the number of slices
  inline Size numSlices() const noexcept { return storage_.size()[2]; }

  /// @brief Returns the size of the image stack
  inline auto size() const noexcept { return storage_.size(); }

  /// @brief Returns a container of unique values in ascending order found
  /// inside the image stack.
  /// @return container containing all unique values of the image in ascending
  /// order
  /// @note This method requires to map the underlying storage.
  auto uniqueValues() const {
    std::set<StorageType> values;

    if (empty()) return values;

    for (auto const &v : storage_.map()) values.insert(v);

    Ensures(std::is_sorted(values.cbegin(), values.cend()));

    return values;
  }

  /// @brief Returns true if the image is empty
  inline bool empty() const noexcept { return storage_.empty(); }

  /// @brief Maps the underlying storage to host memory and returns a const
  /// mapping object
  /// @pre `empty() == false`
  /// @return A const mapping object
  inline auto map() const noexcept(noexcept(std::declval<Storage>().map())) {
    return storage_.map();
  }

  /// @brief Maps the underlying storage to host memory and returns a
  /// mapping object
  /// @pre `empty() == false`
  /// @return A mapping object
  inline auto map() noexcept(noexcept(std::declval<Storage>().map())) {
    return storage_.map();
  }

private:
  template <class, template <class> class, class... Decs>
  friend class ImageStack;

  Storage storage_;
};

#pragma clang diagnostic pop

/// @{
/// @ingroup TypeTraits

/// @brief Type trait to check if a ImageStack implements a given decorator
template <class T, class D> struct HasDecorator : public std::false_type {};

template <class D, class T, template <class> class S, class... _D>
struct HasDecorator<ImageStack<T, S, _D...>, D>
    : public ContainsType<D, _D...> {};

/// @brief Alias for HasDecorator<>::value
template <class T, class D>
constexpr bool hasDecorator_v = HasDecorator<T, D>::type;

/// @}

} // namespace ImageStack

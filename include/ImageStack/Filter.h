#pragma once

#include "ImageStack.h"

namespace ImageStack {

namespace Filter {

template <class Derived> struct Traits {};

template <class Derived> class FilterBase {
public:
  using Scalar = typename Traits<Derived>::Scalar;

  inline operator Derived &() { return static_cast<Derived &>(*this); }

  inline operator Derived const &() const {
    return static_cast<Derived const &>(*this);
  }

  inline Size3 size() const {
    return static_cast<Derived const &>(*this).size();
  }

  inline Size3 halfSize() const { return (size() - Size3::Ones()) / 2; }

  template <class Idx, typename = std::enable_if_t<isModelOfMultiIndex_v<Idx> &&
                                                   (dims_v<Idx> >= 3)>>
  inline Scalar const &operator[](Idx &&i) const {
    return static_cast<Derived const &>(*this)[std::forward<Idx>(i)];
  }
};

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wweak-vtables"
struct FilterException : public std::runtime_error {
  explicit FilterException(std::string const &what) : runtime_error{what} {}
};
#pragma clang diagnostic pop

#pragma clang diagnostic ignored "-Wunused-variable"
template <class Derived, class T, class... Decorators>
auto filter(ImageStack<T, HostStorage, Decorators...> const &img,
            FilterBase<Derived> const &filter, bool pad = true) {

  using Img = ImageStack<T, HostStorage, Decorators...>;

  SIndex3 const K = filter.halfSize().template cast<long>();

  if (!pad && (img.size()[0] <= narrow_cast<std::size_t>(2 * K[0]) ||
               img.size()[1] <= narrow_cast<std::size_t>(2 * K[1]) ||
               img.size()[2] <= narrow_cast<std::size_t>(2 * K[2]))) {

    std::stringstream imgSize;
    std::stringstream filterSize;
    imgSize << img.size().transpose();
    filterSize << (2 * (K + SIndex3::Ones())).transpose();

    throw FilterException{"Image of size " + imgSize.str() +
                          " too small for filter size " + filterSize.str()};
  }

  Size3 const finalSize = pad ? img.size()
                              : (img.size().template cast<long>() - 2 * K)
                                    .template cast<std::size_t>();

  Img dest{finalSize, T{0}};

  auto const mSrc = img.map();
  auto mDest = dest.map();

  auto const inside = [&finalSize](auto const &x) {
    return x.x() >= 0 && x.y() >= 0 && x.z() >= 0 &&
           x.x() < narrow_cast<long>(finalSize.x()) &&
           x.y() < narrow_cast<long>(finalSize.y()) &&
           x.z() < narrow_cast<long>(finalSize.z());
  };

#pragma clang diagnostic ignored "-Wsource-uses-openmp"
#pragma omp parallel for
  for (long k = 0; k < narrow_cast<long>(finalSize[2]); ++k) {
    for (auto j = 0; j < narrow_cast<long>(finalSize[1]); ++j) {
      for (auto i = 0; i < narrow_cast<long>(finalSize[0]); ++i) {

        SIndex3 const x{i, j, k};

        for (auto c = -K[2]; c <= K[2]; ++c) {
          for (auto b = -K[1]; b <= K[1]; ++b) {
            for (auto a = -K[0]; a <= K[0]; ++a) {

              SIndex3 const y{a, b, c};

              auto const srcVal =
                  pad ? (inside(x - y) ? mSrc[x - y] : T{0}) : mSrc[x + K - y];

              mDest[x] += srcVal * filter[y];
            }
          }
        }
      }
    }
  }

  return dest;
}

} // namespace Filter
} // namespace ImageStack

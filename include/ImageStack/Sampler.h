#pragma once

#include "ImageStack.h"
#include "ResolutionDecorator.h"
#include "TypeTraits.h"

namespace ImageStack {
namespace Sampler {

template <class DerivedSampler> class SamplerBase {
public:
  template <class T, template <class> class Storage, class... Decorators,
            class Derived,
            typename = std::enable_if_t<isHostStorage_v<Storage>>>
  inline decltype(auto)
  operator()(ImageStack<T, Storage, Decorators...> const &img,
             Eigen::MatrixBase<Derived> const &pos) const {
    auto const map = img.map();
    return static_cast<DerivedSampler const &>(*this).at(img, map, pos);
  }

  template <class T, template <class> class Storage, class... Decorators,
            class InputIterator, class OutputIterator,
            typename = std::enable_if_t<
                std::is_base_of<std::input_iterator_tag,
                                typename std::iterator_traits<
                                    InputIterator>::iterator_category>::value &&
                isHostStorage_v<Storage>>>
  inline void operator()(ImageStack<T, Storage, Decorators...> const &img,
                         InputIterator begin, InputIterator end,
                         OutputIterator out) const {
    auto const map = img.map();

    for (auto i = begin; i != end; ++i, ++out)
      *out = static_cast<DerivedSampler const &>(*this).at(img, map, *i);
  }

  template <class T, template <class> class Storage, class... Decorators,
            class Container,
            typename = std::enable_if_t<isContainer_v<Container> &&
                                        isHostStorage_v<Storage>>>
  inline decltype(auto)
  operator()(ImageStack<T, Storage, Decorators...> const &img,
             Container const &positions) const {
    using ResultType = std::remove_reference_t<decltype(
        static_cast<DerivedSampler const &>(*this).at(img, img.map(),
                                                      *positions.begin()))>;

    std::vector<ResultType> results(positions.size());

    (*this)(img, std::begin(positions), std::end(positions), results.begin());

    return results;
  }

protected:
  SamplerBase() = default;
};

template <class T, class DerivedSampler = void>
class BasicSampler
    : public SamplerBase<
          std::conditional_t<std::is_same<DerivedSampler, void>::value,
                             BasicSampler<T>, DerivedSampler>> {
  friend class SamplerBase<BasicSampler<T>>;

public:
  inline BasicSampler() = default;

  inline BasicSampler(T value) : outsideValue_(value) {}

protected:
  template <class Img, class Map, class Derived>
  inline decltype(auto) at(Img const &img, Map const &map,
                           Eigen::MatrixBase<Derived> const &pos) const {
    auto const size = img.size();
    Index3 location;
    if (static_cast<Size>(pos(0)) >= size(0) || static_cast<Size>(pos(0)) < 0 ||
        static_cast<Size>(pos(1)) >= size(1) || static_cast<Size>(pos(1)) < 0 ||
        static_cast<Size>(pos(2)) >= size(2) || static_cast<Size>(pos(2)) < 0)
      return outsideValue_;

    return static_cast<T>(map[pos.template head<3>().template cast<Index>()]);
  }

private:
  T outsideValue_{static_cast<T>(0)};
};

template <template <class> class Base>
class ResolutionSampler : public Base<ResolutionSampler<Base>> {
public:
  using Base<ResolutionSampler>::Base;

protected:
  template <class Img, class Map, class Derived>
  inline auto at(Img const &img, Map const &map,
                 Eigen::MatrixBase<Derived> const &pos) const {

    static_assert(hasDecorator_v<Img, ResolutionDecorator>,
                  "Image type has not resolution decorator");

    auto const res = img.resolution;

    SIndex3 const location(static_cast<SIndex>(pos(0) / res(0)),
                           static_cast<SIndex>(pos(1) / res(1)),
                           static_cast<SIndex>(pos(2) / res(2)));

    return Base<ResolutionSampler>::at(img, map, location);
  }
};

} // namespace Sampler
} // namespace ImageStack

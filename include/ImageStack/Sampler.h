#pragma once

#include "ImageStack.h"
#include "ResolutionDecorator.h"
#include "TypeTraits.h"

namespace ImageStack {
namespace Sampler {

namespace CoordTransform {

struct Identity {
  template <class T, template <class> class Storage, class... Decorators,
            class Map, class Derived,
            typename = std::enable_if_t<isHostStorage_v<Storage>>>
  inline decltype(auto)
  transformCoord(ImageStack<T, Storage, Decorators...> const &, Map const &,
                 Eigen::MatrixBase<Derived> const &pos) const noexcept {
    return static_cast<Derived const &>(pos);
  }
};

struct ResolutionScale {
  template <class T, template <class> class Storage, class... Decorators,
            class Map, class Derived,
            typename = std::enable_if_t<isHostStorage_v<Storage>>>
  inline decltype(auto)
  transformCoord(ImageStack<T, Storage, Decorators...> const &img, Map const &,
                 Eigen::MatrixBase<Derived> const &pos) const noexcept {
    using IS = ImageStack<T, Storage, Decorators...>;
    static_assert(hasDecorator_v<IS, ResolutionDecorator>,
                  "ImageStack has no resolution decorator");

    return pos.template cast<double>().cwiseQuotient(img.resolution);
  }
};

} // namespace CoordTransform

namespace Interpolation {

template <class BorderPolicy> struct Nearest : public BorderPolicy {

  template <class T, template <class> class Storage, class... Decorators,
            class Map, class Derived,
            typename = std::enable_if_t<isHostStorage_v<Storage>>>
  inline decltype(auto)
  interpolate(ImageStack<T, Storage, Decorators...> const &img, Map const &map,
              Eigen::MatrixBase<Derived> const &pos) const {
    return interpolate(
        img, map, pos,
        std::integral_constant<
            bool, std::is_integral<typename Derived::Scalar>::value>{});
  }

private:
  /// @brief Nearest interpolation for integer coordinates, i.e. identity
  template <class Img, class Map, class Derived>
  inline decltype(auto) interpolate(Img const &img, Map const &map,
                                    Eigen::MatrixBase<Derived> const &pos,
                                    std::true_type) const {
    return BorderPolicy::value(img, map, pos.template cast<SIndex>());
  }

  /// @brief Nearest interpolation for real coordinates
  template <class Img, class Map, class Derived>
  inline decltype(auto) interpolate(Img const &img, Map const &map,
                                    Eigen::MatrixBase<Derived> const &pos,
                                    std::false_type) const {
    using Scalar = typename Derived::Scalar;
    using std::round;
    static_assert(std::is_floating_point<Scalar>::value,
                  "Only available for real coordinates");

    SIndex3 const rounded(static_cast<SIndex>(round(pos(0))),
                          static_cast<SIndex>(round(pos(1))),
                          static_cast<SIndex>(round(pos(2))));
    return BorderPolicy::value(img, map, rounded);
  }
};

template <class BorderPolicy> struct Linear : public BorderPolicy {

  template <class T, template <class> class Storage, class... Decorators,
            class Map, class Derived,
            typename = std::enable_if_t<isHostStorage_v<Storage>>>
  inline decltype(auto)
  interpolate(ImageStack<T, Storage, Decorators...> const &img, Map const &map,
              Eigen::MatrixBase<Derived> const &pos) const {
    return interpolate(
        img, map, pos,
        std::integral_constant<
            bool, std::is_integral<typename Derived::Scalar>::value>{});
  }

private:
  /// @brief Linear interpolation for integer coordinates, i.e. identity
  template <class Img, class Map, class Derived>
  inline decltype(auto) interpolate(Img const &img, Map const &map,
                                    Eigen::MatrixBase<Derived> const &pos,
                                    std::true_type) const {
    return BorderPolicy::value(img, map, pos.template cast<SIndex>());
  }

  /// @brief Linear interpolation for real coordinates
  template <class Img, class Map, class Derived>
  inline decltype(auto) interpolate(Img const &img, Map const &map,
                                    Eigen::MatrixBase<Derived> const &pos,
                                    std::false_type) const {
    using std::floor;
    using Scalar = typename Derived::Scalar;
    using V3 = Eigen::Matrix<Scalar, 3, 1>;
    static_assert(std::is_floating_point<Scalar>::value,
                  "Only available for real coordinates");

    SIndex3 const p0(static_cast<SIndex>(floor(pos(0))),
                     static_cast<SIndex>(floor(pos(1))),
                     static_cast<SIndex>(floor(pos(2))));
    V3 const pd = (pos - p0.template cast<Scalar>());

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdouble-promotion"
    // Interpolate along x-axis
    auto const v00 = this->value(img, map, p0) * (1.0 - pd(0)) +
                     this->value(img, map, p0 + SIndex3::UnitX()) * pd(0);
    auto const v01 =
        this->value(img, map, p0 + SIndex3::UnitZ()) * (1.0 - pd(0)) +
        this->value(img, map, p0 + SIndex3::UnitZ() + SIndex3::UnitX()) * pd(0);
    auto const v10 =
        this->value(img, map, p0 + SIndex3::UnitY()) * (1.0 - pd(0)) +
        this->value(img, map, p0 + SIndex3::UnitY() + SIndex3::UnitX()) * pd(0);
    auto const v11 =
        this->value(img, map, p0 + SIndex3::UnitY() + SIndex3::UnitZ()) *
            (1.0 - pd(0)) +
        this->value(img, map, p0 + SIndex3::UnitY() + SIndex3::UnitZ() +
                                  SIndex3::UnitX()) *
            pd(0);

    // interpoalte along y-axis
    auto const v0 = v00 * (1.0 - pd(1)) + v10 * pd(1);
    auto const v1 = v01 * (1.0 - pd(1)) + v11 * pd(1);

    /// interpolate along z-axis
    auto const v = v0 * (1.0 - pd(2)) + v1 * pd(2);
#pragma clang diagnostic pop

    return v;
  }
};

} // namespace Interpolation

namespace BorderPolicy {

struct FixedValue {

  double outside{static_cast<double>(0)};

  template <class S, template <class> class Storage, class... Decorators,
            class Map, class Derived,
            typename = std::enable_if_t<isHostStorage_v<Storage> &&
                                        std::is_arithmetic<S>::value>>
  inline auto value(ImageStack<S, Storage, Decorators...> const &img,
                    Map const &map,
                    Eigen::MatrixBase<Derived> const &pos) const {

    static_assert(std::is_integral<typename Derived::Scalar>::value,
                  "Coordinates must be integral");

    auto const size = img.size().template cast<typename Derived::Scalar>();
    if (pos(0) < 0 || pos(0) >= size(0) || pos(1) < 0 || pos(1) >= size(1) ||
        pos(2) < 0 || pos(2) >= size(2)) {
      return static_cast<S>(outside);
    }

    return map[pos.template cast<Index>()];
  }
};

} // namespace BorderPolicy

namespace ValueTransform {

struct Identity {
  template <class T>
  inline decltype(auto) transformValue(T const &v) const noexcept {
    return v;
  }
};

} // namespace ValueTransform

template <class CoordTransform = CoordTransform::Identity,
          template <class> class Interpolation = Interpolation::Nearest,
          class BorderPolicy = BorderPolicy::FixedValue,
          class ValueTransform = ValueTransform::Identity>
class Sampler : public CoordTransform,
                public Interpolation<BorderPolicy>,
                public ValueTransform {
public:
  template <class T, template <class> class Storage, class... Decorators,
            class Derived,
            typename = std::enable_if_t<isHostStorage_v<Storage>>>
  inline decltype(auto)
  operator()(ImageStack<T, Storage, Decorators...> const &img,
             Eigen::MatrixBase<Derived> const &pos) const {

    auto const map = img.map();

    return at(img, map, pos);
  }

  template <class T, template <class> class Storage, class... Decorators,
            class InputIterator, class OutputIterator,
            typename = std::enable_if_t<
                isHostStorage_v<Storage> &&
                std::is_base_of<std::input_iterator_tag,
                                typename std::iterator_traits<
                                    InputIterator>::iterator_category>::value>>
  inline void operator()(ImageStack<T, Storage, Decorators...> const &img,
                         InputIterator begin, InputIterator end,
                         OutputIterator out) const {
    auto const map = img.map();

    std::transform(begin, end, out, [this, &img, &map](auto const &x) {
      return this->at(img, map, x);
    });
  }

  template <class T, template <class> class Storage, class... Decorators,
            class Positions,
            typename = std::enable_if_t<
                isHostStorage_v<Storage> && isContainer_v<Positions> &&
                isEigenMatrix_v<typename Positions::value_type>>>
  inline decltype(auto)
  operator()(ImageStack<T, Storage, Decorators...> const &img,
             Positions const &positions) const {

    using ResultType =
        std::decay_t<decltype(at(img, img.map(), *positions.begin()))>;

    Eigen::Matrix<ResultType, Eigen::Dynamic, 1> results(
        narrow_cast<long>(positions.size()));

    operator()(img, positions.begin(), positions.end(), results.data());

    return results;
  }

private:
  template <class Img, class Map, class Derived>
  inline decltype(auto) at(Img const &img, Map const &map,
                           Eigen::MatrixBase<Derived> const &pos) const {

    return ValueTransform::transformValue(
        Interpolation<BorderPolicy>::interpolate(
            img, map, CoordTransform::transformCoord(img, map, pos)));
  }
};

} // namespace Sampler
} // namespace ImageStack

#ifndef SAMPLER_HH
#define SAMPLER_HH

#include "ImageStack.hh"
#include "ResolutionDecorator.hh"

namespace SITools {

/// A sampler that returns a predefined value for all integer coordinates
/// outside the image domain
template <class StorageType_> struct ClampSampler {
  typedef int coord_type;
  typedef StorageType_ output_type;
  StorageType_ outsideValue;

  /// Creates a sampler that returns 0 for out of bounds queries
  ClampSampler() : ClampSampler(0) {}
  /// Creates a sampler that returns outsideValue for our of bounds queries
  ClampSampler(StorageType_ outsideValue_ = 0) : outsideValue(outsideValue_) {}

  template <class... Decorators>
  inline StorageType_
  operator()(ImageStack<StorageType_, Decorators...> const &image,
             BIAS::Vector3<coord_type> const &pos) const {
    auto const size = image.Size();
    return (pos[0] >= 0 && pos[0] < static_cast<int>(size[0]) && pos[1] >= 0 &&
            pos[1] < static_cast<int>(size[1]) && pos[2] >= 0 &&
            pos[2] < static_cast<int>(size[2]))
               ? image(pos)
               : outsideValue;
  }
};

/// A sampler that applies a scale transformation for the sample co-ordinates
/// before sampling. The scale is taken as the inverse of the image's
/// resolution. Therefore this sampler is only available for ImageStacks with
/// the Resolution decorator.
template <class ImageStack, class... Sampler> struct ResolutionSampler {
  static_assert(sizeof...(Sampler) < 2,
                "Only zero or one arguments for Sampler are allowed");
};

/// Standalone resolution sampler
template <class ImageStack> struct ResolutionSampler<ImageStack> {
  typedef double coord_type;
  typedef typename ImageStack::StorageType output_type;

  static_assert(
      HasDecorator<ImageStack, ResolutionDecorator>::value,
      "ResolutionSampler needs an ImageStack with a ResolutionDecorator");

  inline output_type operator()(ImageStack const &image,
                                BIAS::Vector3<coord_type> const &pos) const {
    auto const &res = image.resolution;
    return image(static_cast<unsigned int>(pos[0] / res[0]),
                 static_cast<unsigned int>(pos[1] / res[1]),
                 static_cast<unsigned int>(pos[2] / res[2]));
  }
};

/// Delegating resolution sampler, i.e. the sampling process is delegated to
/// another sampler
template <class ImageStack, class Sampler>
struct ResolutionSampler<ImageStack, Sampler> : public Sampler {
  typedef double coord_type;
  typedef typename Sampler::output_type output_type;

  static_assert(
      HasDecorator<ImageStack, ResolutionDecorator>::value,
      "ResolutionSampler needs an ImageStack with a ResolutionDecorator");

  template <class... Args>
  ResolutionSampler(Args &&... args)
      : Sampler(std::forward<Args>(args)...) {}

  inline output_type operator()(ImageStack const &image,
                                BIAS::Vector3<coord_type> const &pos) const {
    typedef typename Sampler::coord_type CT;
    auto const &res = image.resolution;
    BIAS::Vector3<CT> scaledPos(CT(pos[0] / res[0]), CT(pos[1] / res[1]),
                                CT(pos[2] / res[2]));
    return static_cast<Sampler const &>(*this)(image, scaledPos);
  }
};

/// A sampler that transforms the result of another sampler using a function
template <class Sampler, class Fun>
struct TransformingSampler : public Sampler {
  typedef typename std::result_of<Fun(typename Sampler::output_type)>::type
      output_type;
  typedef typename Sampler::coord_type coord_type;

  template <class... Args>
  TransformingSampler(Fun f, Args &&... args)
      : Sampler(std::forward<Args>(args)...), function(f) {}

  template <class ImageStack>
  inline output_type operator()(ImageStack const &image,
                                BIAS::Vector3<coord_type> const &pos) const {
    return function(Sampler::operator()(image, pos));
  }

  Fun function;
};

/// A sampler that interpolates the eight surrounding grid points of a sampling
/// co-ordinate. The interpolation scheme and the actual sampler can configure.
template <class CoordType, class Sampler, class Interpolator>
struct InterpolatingSampler : public Sampler, private Interpolator {
  typedef CoordType coord_type;
  typedef typename Interpolator::output_type output_type;

  template <class... Args>
  InterpolatingSampler(Args &&... args)
      : Sampler(std::forward<Args>(args)...) {}

  template <class IS>
  typename Interpolator::output_type
  operator()(IS const &image, BIAS::Vector3<coord_type> const &pos) const {
    typedef typename Sampler::coord_type SCT;
    SCT const x0 = static_cast<SCT>(std::floor(pos[0]));
    SCT const x1 = static_cast<SCT>(std::ceil(pos[0]));
    SCT const y0 = static_cast<SCT>(std::floor(pos[1]));
    SCT const y1 = static_cast<SCT>(std::ceil(pos[1]));
    SCT const z0 = static_cast<SCT>(std::floor(pos[2]));
    SCT const z1 = static_cast<SCT>(std::ceil(pos[2]));
    auto const v000 = static_cast<Sampler const &>(*this)(image, {x0, y0, z0});
    auto const v001 = static_cast<Sampler const &>(*this)(image, {x0, y0, z1});
    auto const v010 = static_cast<Sampler const &>(*this)(image, {x0, y1, z0});
    auto const v011 = static_cast<Sampler const &>(*this)(image, {x0, y1, z1});
    auto const v100 = static_cast<Sampler const &>(*this)(image, {x1, y0, z0});
    auto const v101 = static_cast<Sampler const &>(*this)(image, {x1, y0, z1});
    auto const v110 = static_cast<Sampler const &>(*this)(image, {x1, y1, z0});
    auto const v111 = static_cast<Sampler const &>(*this)(image, {x1, y1, z1});
    auto const res = static_cast<Interpolator const &>(*this)(
        pos[0] - x0, pos[1] - y0, pos[2] - z0, v000, v001, v010, v011, v100,
        v101, v110, v111);
    // std::cout << pos << " => " << res << std::endl;
    return res;
  }
};

/// A sampler that interpolates the eight surrounding grid points of a sampling
/// co-ordinate using weights from a weight image. The interpolation scheme and
/// the actual sampler can be configured.
template <class CoordType, class Sampler, class Interpolator,
          class WeightFunction>
struct WeightedInterpolatingSampler : public Sampler, private Interpolator {
  typedef CoordType coord_type;
  typedef typename Interpolator::output_type output_type;

  template <class... Args>
  WeightedInterpolatingSampler(WeightFunction fun, Args &&... args)
      : Sampler(std::forward<Args>(args)...), weight(fun) {}

  template <class IS>
  typename Interpolator::output_type
  operator()(IS const &image, BIAS::Vector3<coord_type> const &pos) const {
    typedef typename Sampler::coord_type SCT;
    SCT const x0 = std::floor(pos[0]);
    SCT const x1 = std::ceil(pos[0]);
    SCT const y0 = std::floor(pos[1]);
    SCT const y1 = std::ceil(pos[1]);
    SCT const z0 = std::floor(pos[2]);
    SCT const z1 = std::ceil(pos[2]);
    double w000 = weight(BIAS::Vector3<SCT>(x0, y0, z0));
    double w001 = weight(BIAS::Vector3<SCT>(x0, y0, z1));
    double w010 = weight(BIAS::Vector3<SCT>(x0, y1, z0));
    double w011 = weight(BIAS::Vector3<SCT>(x0, y1, z1));
    double w100 = weight(BIAS::Vector3<SCT>(x1, y0, z0));
    double w101 = weight(BIAS::Vector3<SCT>(x1, y0, z1));
    double w110 = weight(BIAS::Vector3<SCT>(x1, y1, z0));
    double w111 = weight(BIAS::Vector3<SCT>(x1, y1, z1));

    auto const sum = w000 + w001 + w010 + w011 + w100 + w101 + w110 + w111;
    if (sum > 0) {
      w000 /= sum;
      w001 /= sum;
      w010 /= sum;
      w011 /= sum;
      w100 /= sum;
      w101 /= sum;
      w110 /= sum;
      w111 /= sum;
    }

    double const v000 =
        w000 * static_cast<Sampler const &>(*this)(image, {x0, y0, z0});
    double const v001 =
        w001 * static_cast<Sampler const &>(*this)(image, {x0, y0, z1});
    double const v010 =
        w010 * static_cast<Sampler const &>(*this)(image, {x0, y1, z0});
    double const v011 =
        w011 * static_cast<Sampler const &>(*this)(image, {x0, y1, z1});
    double const v100 =
        w100 * static_cast<Sampler const &>(*this)(image, {x1, y0, z0});
    double const v101 =
        w101 * static_cast<Sampler const &>(*this)(image, {x1, y0, z1});
    double const v110 =
        w110 * static_cast<Sampler const &>(*this)(image, {x1, y1, z0});
    double const v111 =
        w111 * static_cast<Sampler const &>(*this)(image, {x1, y1, z1});
    auto const res = static_cast<Interpolator const &>(*this)(
        pos[0] - x0, pos[1] - y0, pos[2] - z0, v000, v001, v010, v011, v100,
        v101, v110, v111);
    return res;
  }

  WeightFunction weight;
};

/// A tri-linear interpolator that can be used in the InterpolatingSampler.
template <class T = double> struct TrilinearInterpolator {
  typedef T output_type;
  template <class CoordType, class ValueType,
            class OutputType = decltype(std::declval<CoordType>() *
                                        std::declval<output_type>())>
  inline OutputType operator()(CoordType xd, CoordType yd, CoordType zd,
                               ValueType v000, ValueType v001, ValueType v010,
                               ValueType v011, ValueType v100, ValueType v101,
                               ValueType v110, ValueType v111) const {
    // interpolate along x-axis
    auto const v00 = static_cast<output_type>(v000) * (1 - xd) +
                     static_cast<output_type>(v100) * xd;
    auto const v01 = static_cast<output_type>(v001) * (1 - xd) +
                     static_cast<output_type>(v101) * xd;
    auto const v10 = static_cast<output_type>(v010) * (1 - xd) +
                     static_cast<output_type>(v110) * xd;
    auto const v11 = static_cast<output_type>(v011) * (1 - xd) +
                     static_cast<output_type>(v111) * xd;

    // interpolate along y-axis
    auto const v0 = v00 * (1 - yd) + v10 * yd;
    auto const v1 = v01 * (1 - yd) + v11 * yd;

    // interpolate along z-axis
    auto const v = v0 * (1 - zd) + v1 * zd;

    return v;
  }
};

} // namespace SITools

#endif // SAMPLER_HH

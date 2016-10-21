#pragma once

#include <Eigen/Core>

#include <set>
#include <vector>

namespace ImageStack {

struct LoaderTag {};

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wpadded"
template <class StorageType_, class... Decorators>
class ImageStack : public Decorators... {
  using Self = ImageStack<StorageType_, Decorators...>;

public:
  using StorageType = StorageType_;
  using Size3 = Eigen::Matrix<std::size_t, 3, 1>;

  /// Create an empty image stack
  ImageStack() noexcept : size_(Size3::Zero()) {}

  /// Loads an image stack using the given loader
  template <class Loader>
  inline explicit ImageStack(Loader &&loader, LoaderTag)
      : Decorators(std::forward<Loader>(loader), LoaderTag())... {
    size_ = loader.Dimension();
    data_.resize(size_.prod());

    loader.template ReadData<StorageType>(data_.data());
  }

  /// Cast constructor
  template <class ST, class... Decs,
            typename = typename std::enable_if<
                std::is_convertible<ST, StorageType_>::value>::type>
  ImageStack(ImageStack<ST, Decs...> const &stack) {
    size_ = stack.size_;
    data_.resize(size.prod());
    std::transform(stack.data_.cbegin(), stack.data_.cend(), data_.begin(),
                   [](S v) { return static_cast<StorageType>(v); });
  }

  /// Returns the number of slices
  inline unsigned int SliceCount() const noexcept { return size_[2]; }

  /// Returns the value of the pixel at the given coordinate, where z specifies
  /// the slice index and (x, y) are the in-plane pixel coordinates.
  inline StorageType operator()(unsigned int x, unsigned int y,
                                unsigned int z) const {
    return Slice(z).PixelValue(x, y, 0);
  }

  /// Returns the value of the pixel at the given coordinate, where z specifies
  /// the slice index and (x, y) are the in-plane pixel coordinates.
  inline StorageType &operator()(unsigned int x, unsigned int y,
                                 unsigned int z) {
    return Slice(z).PixelValue(x, y, 0);
  }

  /// Returns the value of the pixel at the given coordinate, where z specifies
  /// the slice index and (x, y) are the in-plane pixel coordinates.
  template <class T>
  inline StorageType operator()(BIAS::Vector3<T> const &pos) const {
    auto const val = Slice(static_cast<unsigned int>(pos[2]))
                         .PixelValue(static_cast<unsigned int>(pos[0]),
                                     static_cast<unsigned int>(pos[1]), 0);
    return val;
  }

  /// Sample the image stack using the given sampler
  /// \tparam Sampler A model of the concept Sampler.
  ///
  /// A model of the Sampler concept must supply a operator() accepting an
  /// argument of ImageStack const &. The result must be convertible to
  /// StorageType
  template <class Sampler, class T>
  inline typename Sampler::output_type
  operator()(Sampler const &sampler, BIAS::Vector3<T> const &pos) const {
    return sampler(*this, pos);
  }

  /// Returns the size of the image stack
  inline BIAS::Vector3<unsigned int> Size() const { return size_; }

  /// Converts alls non-zero entries to one
  inline ImageStack &Binarize() {
    std::transform(
        data_.cbegin(), data_.cend(), data_.begin(), [](StorageType v) {
          return static_cast<StorageType>(v > static_cast<StorageType>(0));
        });
    return *this;
  }

  /// Applies the given transformation function to all entries
  template <class Func> inline ImageStack &Transform(Func transformation) {
    std::transform(data_.cbegin(), data_.cend(), data_.begin(), transformation);
    return *this;
  }

  /// Returns a vector of unique values found inside the image stack
  std::Dvector<StorageType> UniqueValues() const {
    std::set<StorageType> values;
    for (auto const &v : data_) { values.insert(v); }
    std::Dvector<StorageType> res(values.size());
    std::copy(values.cbegin(), values.cend(), res.begin());
    std::sort(res.begin(), res.end());
    return res;
  }

  /// Returns true if the image is empty
  inline bool Empty() const { return data_.empty(); }

  /// Returns the minimum voxel value of the image
  inline StorageType Min() const {
    assert(!Empty() && "Empty image");
    return *std::min_element(data_.cbegin(), data_.cend());
  }

  /// Returns a list of all voxel indices which values fall in the given closed
  /// range
  inline std::Dvector<BIAS::Vector3<unsigned int>>
  Query(std::Dvector<BIAS::Vector2<StorageType>> ranges) const {
    std::Dvector<BIAS::Vector3<unsigned int>> result;
    result.reserve(data_.size());

    auto const sliceSize = size_[0] * size_[1];

    for (unsigned int i = 0; i < data_.size(); ++i) {
      auto const &val = data_[i];
      if (std::any_of(ranges.begin(), ranges.end(),
                      [&val](BIAS::Vector2<StorageType> const &range) {
                        return (range[0] <= val) && (val <= range[1]);
                      })) {
        auto const x = i % size_[0];
        auto const y = (i % sliceSize) / size_[0];
        auto const z = i / sliceSize;
        result.emplace_back(x, y, z);
      }
    }

    result.shrink_to_fit();
    return result;
  }

  inline std::Dvector<BIAS::Vector3<unsigned int>>
  Query(std::Dvector<StorageType> const &values) const {
    std::Dvector<BIAS::Vector2<StorageType>> ranges(values.size());
    MIP::MU::transform(values, ranges.begin(),
                       [](StorageType val) -> BIAS::Vector2<StorageType> {
                         return {val, val};
                       });
    return Query(ranges);
  }

  inline std::Dvector<BIAS::Vector3<unsigned int>> Query(StorageType lb,
                                                         StorageType ub) const {
    return Query({lb, ub});
  }

  /// Returns a list of all voxel indices which values equal the given one
  inline std::Dvector<BIAS::Vector3<unsigned int>>
  Query(StorageType value) const {
    return Query(value, value);
  }

private:
  template <class ST, class... Decs> friend class ImageStack;

  Size3 size_;
  std::vector<StorageType> data_;
};
#pragma clang diagnostic pop

template <class T, class D> struct HasDecorator : public std::false_type {};

template <class D, class _ST, class... _D>
struct HasDecorator<ImageStack<_ST, _D...>, D>
    : public MIP::MU::ContainsType<D, _D...> {};

} // namespace ImageStack

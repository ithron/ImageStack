#pragma once

#include "Filter.h"

namespace ImageStack {
namespace Filter {

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wpadded"
template <class T = double, SIndex W = Dynamic, SIndex H = Dynamic,
          SIndex D = Dynamic>
class GaussFilter : public FilterBase<GaussFilter<T, W, H, D>> {
  using Parent = FilterBase<GaussFilter<T, W, H, D>>;

  friend Parent;

public:
  inline auto size() const noexcept { return size_; }

  inline auto sigma() const noexcept { return sigma_; }

  template <class Derived>
  inline GaussFilter(Eigen::MatrixBase<Derived> const &sigma, SIndex w = W,
                     SIndex h = H, SIndex d = D)
      : sigma_{sigma} {
    auto const w_ = (w == Dynamic ? static_cast<Size>(ceil(T{3.0} * sigma[0]))
                                  : narrow_cast<Size>((w - 1) / 2));
    auto const h_ = (h == Dynamic ? static_cast<Size>(ceil(T{3.0} * sigma[1]))
                                  : narrow_cast<Size>((h - 1) / 2));
    auto const d_ = (d == Dynamic ? static_cast<Size>(ceil(T{3.0} * sigma[2]))
                                  : narrow_cast<Size>((d - 1) / 2));

    Expects(w_ >= 0 && h_ >= 0 && d_ >= 0);

    size_ = 2 * Size3{w_, h_, d_} + Size3::Ones();

    SIndex3 const K = Size3{w_, h_, d_}.cast<long>();

    constexpr auto WH = (W == Dynamic || H == Dynamic) ? Dynamic : W * H;

    Eigen::Matrix<T, WH, D> weights = Eigen::Matrix<T, WH, D>::Zero(
        narrow_cast<long>(size_[0] * size_[1]), narrow_cast<long>(size_[2]));

    Eigen::Matrix<T, 3, 1> const sigmaSqInv =
        sigma.cwiseProduct(sigma).cwiseInverse();

    for (auto k = -K[2]; k <= K[2]; ++k) {
      for (auto j = -K[1]; j <= K[1]; ++j) {
        for (auto i = -K[0]; i <= K[0]; ++i) {
          auto const x = SIndex3{i, j, k}.template cast<T>().eval();
          weights((j + K[1]) * narrow_cast<long>(size_[0]) + i + K[0],
                  k + K[2]) =
              exp(-T{0.5} * x.transpose() * sigmaSqInv.asDiagonal() * x);
        }
      }
    }

    weights /= weights.sum();

    weights_ = std::move(weights);

    K_ = K;
  }

  template <class Idx, typename = std::enable_if_t<isModelOfMultiIndex_v<Idx> &&
                                                   (dims_v<Idx> >= 3)>>
  inline T const &operator[](Idx &&i) const {
    return weights_((i[1] + K_[1]) * narrow_cast<long>(size_[0]) + i[0] + K_[0],
                    i[2] + K_[2]);
  }

private:
  Eigen::Matrix<T, 3, 1> sigma_;
  Eigen::Matrix<T, (W == Dynamic || H == Dynamic) ? Dynamic : W * H, D>
      weights_;
  Size3 size_;
  SIndex3 K_;
};

#pragma clang diagnostic pop

template <class T, SIndex W, SIndex H, SIndex D>
struct Traits<GaussFilter<T, W, H, D>> {
  using Scalar = T;
};

} // namespace Filter

} // namespace ImageStack


// clang-format off
#ifndef XCODE_BUILD
#  include "config.h"
#else
  static constexpr char kTestDataDir[] = TEST_DATA_DIR;
#endif
// clang-format on

#include <ImageStack/ImageStack.h>
#include <ImageStack/ImageStackLoaderBST.h>
#include <ImageStack/ResolutionDecorator.h>
#include <ImageStack/Sampler.h>

#include <gtest/gtest.h>

#include <string>

#pragma clang diagnostic ignored "-Wcovered-switch-default"
#pragma clang diagnostic ignored "-Wexit-time-destructors"
#pragma clang diagnostic ignored "-Wglobal-constructors"
#pragma clang diagnostic ignored "-Wmissing-variable-declarations"
#pragma clang diagnostic ignored "-Wused-but-marked-unused"

using namespace ImageStack;
using namespace Sampler;
using namespace std::literals;

static std::string const ascendingImageFile =
    kTestDataDir + "/ascending_Slices.bst"s;
static std::string const ascendingMaskFile =
    kTestDataDir + "/ascending_Mask.bst"s;
static Size3 const ascendingImageSize{20, 40, 10};
static Eigen::Vector3d const ascendingImageResolution{0.25, 0.5, 1.0};

using Img = ::ImageStack::ImageStack<float, HostStorage, ResolutionDecorator>;
using Mask =
    ::ImageStack::ImageStack<std::uint8_t, HostStorage, ResolutionDecorator>;

using ImgLoader = ImageStackLoaderBST<Img, false>;
using MaskLoader = ImageStackLoaderBST<Mask, true>;

TEST(Sampler, BasicSampler) {
  ImgLoader loader(ascendingImageFile);
  Img const img(loader);

  BasicSampler<float> const sampler(INFINITY);

  ASSERT_FALSE(std::isfinite(sampler(img, SIndex3(-1, -1, -1))));
  ASSERT_FALSE(std::isfinite(sampler(img, SIndex3(0, 0, -1))));
  ASSERT_FALSE(std::isfinite(
      sampler(img, SIndex3(0, narrow_cast<SIndex>(img.size()(1)), 0))));
  ASSERT_FALSE(std::isfinite(
      sampler(img, SIndex3(narrow_cast<SIndex>(img.size()(1) + 100), 0, 0))));

  std::vector<float> refValues;
  std::vector<Index3> indices;

  for (Index z = 0; z < img.size()(2); ++z) {
    for (Index y = 0; y < img.size()(1); ++y) {
      for (Index x = 0; x < img.size()(0); ++x) {
        auto const ref = -2000 + static_cast<double>(y) * 0.5 +
                         static_cast<double>(x) * 20.0 +
                         static_cast<double>(z) * 400.0;
        Index3 const idx(x, y, z);
        refValues.push_back(static_cast<float>(ref));
        indices.push_back(idx);
        ASSERT_FLOAT_EQ(ref, sampler(img, idx));
      }
    }
  }

  auto const values = sampler(img, indices);

  for (Size i = 0; i < values.size(); ++i)
    ASSERT_FLOAT_EQ(refValues[i], values[i]);
}

TEST(Sampler, ResolutionSampler) {
  ImgLoader loader(ascendingImageFile);
  Img const img(loader);

  ResolutionSampler<BasicSampler<float>> const sampler(INFINITY);

  ASSERT_FALSE(std::isfinite(sampler(img, SIndex3(-1, -1, -1))));
  ASSERT_FALSE(std::isfinite(sampler(img, SIndex3(0, 0, -1))));
  ASSERT_FALSE(std::isfinite(
      sampler(img, SIndex3(0, narrow_cast<SIndex>(img.size()(1)), 0))));
  ASSERT_FALSE(std::isfinite(
      sampler(img, SIndex3(narrow_cast<SIndex>(img.size()(1) + 100), 0, 0))));

  auto const res = ascendingImageResolution;

  std::vector<float> refValues;
  std::vector<Eigen::Vector3d> indices;

  for (Index z = 0; z < img.size()(2); ++z) {
    for (Index y = 0; y < img.size()(1); ++y) {
      for (Index x = 0; x < img.size()(0); ++x) {
        auto const ref = -2000 + static_cast<double>(y) * 0.5 +
                         static_cast<double>(x) * 20.0 +
                         static_cast<double>(z) * 400.0;
        Eigen::Vector3d const idx(x * res(0), y * res(1), z * res(2));
        refValues.push_back(static_cast<float>(ref));
        indices.push_back(idx);
        ASSERT_FLOAT_EQ(ref, sampler(img, idx));
      }
    }
  }

  auto const values = sampler(img, indices);

  for (Size i = 0; i < values.size(); ++i)
    ASSERT_FLOAT_EQ(refValues[i], values[i]);
}

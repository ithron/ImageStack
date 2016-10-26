#include "config.h"

#include <ImageStack/ImageStack.h>
#include <ImageStack/ImageStackLoaderBST.h>
#include <ImageStack/ResolutionDecorator.h>

#include <gtest/gtest.h>

#include <string>

#pragma clang diagnostic ignored "-Wcovered-switch-default"
#pragma clang diagnostic ignored "-Wexit-time-destructors"
#pragma clang diagnostic ignored "-Wglobal-constructors"
#pragma clang diagnostic ignored "-Wmissing-variable-declarations"
#pragma clang diagnostic ignored "-Wused-but-marked-unused"

using namespace ImageStack;
using namespace std::literals;

static std::string const zeroImageFile = kTestDataDir + "/zero_Slices.bst"s;
static std::string const zeroMaskFile = kTestDataDir + "/zero_Mask.bst"s;
static Size3 const zeroImageSize{20, 40, 10};
static Eigen::Vector3d const zeroImageResolution{1, 2, 4};

static std::string const onesImageFile = kTestDataDir + "/ones_Slices.bst"s;
static std::string const onesMaskFile = kTestDataDir + "/ones_Mask.bst"s;
static Size3 const onesImageSize{20, 40, 10};
static Eigen::Vector3d const onesImageResolution{1, 2, 4};

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

/// Tries to create a BSTLoader object with an invalid file path and test if
/// an exception in throws.
TEST(BSTLoader, FileNotFound) {
  ASSERT_THROW(ImgLoader(""), std::runtime_error);
  ASSERT_THROW(MaskLoader(""), std::runtime_error);
}

/// Creates a BST loader object with path to an existing image and test if
///   - the constructor does not throw any exceptions
///   - `size()` returns the right value
///   = `resolution()` returns the right value
TEST(BSTLoader, ReadImageHeader) {
  ASSERT_NO_THROW(ImgLoader loader(zeroImageFile));
  ASSERT_NO_THROW(ImgLoader loader(onesImageFile));
  ASSERT_NO_THROW(ImgLoader loader(ascendingImageFile));

  {
    ImgLoader loader(zeroImageFile);
    ASSERT_EQ(zeroImageSize, loader.size());
    ASSERT_EQ(zeroImageResolution, loader.resolution());
  }

  {
    ImgLoader loader(onesImageFile);
    ASSERT_EQ(onesImageSize, loader.size());
    ASSERT_EQ(onesImageResolution, loader.resolution());
  }

  {
    ImgLoader loader(ascendingImageFile);
    ASSERT_EQ(ascendingImageSize, loader.size());
    ASSERT_EQ(ascendingImageResolution, loader.resolution());
  }
}

/// Creates a BST loader object with path to an existing mask and test if
///   - the constructor does not throw any exceptions
///   - `size()` returns the right value
///   = `resolution()` returns the right value
TEST(BSTLoader, ReadMaskHeader) {
  ASSERT_NO_THROW(MaskLoader loader(zeroMaskFile));
  ASSERT_NO_THROW(MaskLoader loader(onesMaskFile));
  ASSERT_NO_THROW(MaskLoader loader(ascendingMaskFile));

  {
    MaskLoader loader(zeroMaskFile);
    ASSERT_EQ(zeroImageSize, loader.size());
    ASSERT_EQ(zeroImageResolution, loader.resolution());
  }

  {
    MaskLoader loader(onesMaskFile);
    ASSERT_EQ(onesImageSize, loader.size());
    ASSERT_EQ(onesImageResolution, loader.resolution());
  }

  {
    MaskLoader loader(ascendingMaskFile);
    ASSERT_EQ(ascendingImageSize, loader.size());
    ASSERT_EQ(ascendingImageResolution, loader.resolution());
  }
}

/// Loads the zero test image and tests if
///   - no exception occour
///   - the size is correct
///   - the resolution is correct
///   - all values are 0
TEST(BSTLoader, ZeroImage) {
  ImgLoader loader(zeroImageFile);

  ASSERT_NO_THROW({ Img tmp(loader); });

  Img const img(loader);

  ASSERT_EQ(zeroImageSize, img.size());
  ASSERT_EQ(zeroImageResolution, img.resolution);

  for (auto v : img.map()) ASSERT_EQ(0.0, v);
}

/// Loads the zero test mask and tests if
///   - no exception occour
///   - the size is correct
///   - the resolution is correct
///   - all values are 0
TEST(BSTLoader, ZeroMask) {
  MaskLoader loader(zeroMaskFile);

  ASSERT_NO_THROW({ Mask tmp(loader); });

  Mask const mask(loader);

  ASSERT_EQ(zeroImageSize, mask.size());
  ASSERT_EQ(zeroImageResolution, mask.resolution);

  for (auto v : mask.map()) ASSERT_EQ(0, v);
}

/// Loads the ones test image and tests if
///   - no exception occour
///   - the size is correct
///   - the resolution is correct
///   - all values are 1
TEST(BSTLoader, OnesImage) {
  ImgLoader loader(onesImageFile);

  ASSERT_NO_THROW({ Img tmp(loader); });

  Img const img(loader);

  ASSERT_EQ(onesImageSize, img.size());
  ASSERT_EQ(onesImageResolution, img.resolution);

  for (auto v : img.map()) ASSERT_EQ(1.0, v);
}

/// Loads the ones test mask and tests if
///   - no exception occour
///   - the size is correct
///   - the resolution is correct
///   - all values are 0
TEST(BSTLoader, OnesMask) {
  MaskLoader loader(onesMaskFile);

  ASSERT_NO_THROW({ Mask tmp(loader); });

  Mask const mask(loader);

  ASSERT_EQ(onesImageSize, mask.size());
  ASSERT_EQ(onesImageResolution, mask.resolution);

  for (auto v : mask.map()) ASSERT_EQ(1, v);
}

/// Loads the ascending test image and tests if
///   - no exception occour
///   - the size is correct
///   - the resolution is correct
///   - all values are correct
TEST(BSTLoader, AscendingImage) {
  ImgLoader loader(ascendingImageFile);

  ASSERT_NO_THROW({ Img tmp(loader); });

  Img const img(loader);

  ASSERT_EQ(ascendingImageSize, img.size());
  ASSERT_EQ(ascendingImageResolution, img.resolution);

  auto const map = img.map();
  for (Size z = 0; z < img.size()[2]; ++z) {
    for (Size y = 0; y < img.size()[1]; ++y) {
      for (Size x = 0; x < img.size()[0]; ++x) {
        auto const v = map[Size3(x, y, z)];
        auto const ref = -2000 + static_cast<double>(y) * 0.5 +
                         static_cast<double>(x) * 20.0 +
                         static_cast<double>(z) * 400.0;

        ASSERT_EQ(ref, v);
      }
    }
  }
}

/// Loads the ascending test mask and tests if
///   - no exception occour
///   - the size is correct
///   - the resolution is correct
///   - all values are correct
TEST(BSTLoader, AscendingMask) {
  MaskLoader loader(ascendingMaskFile);

  ASSERT_NO_THROW({ Mask tmp(loader); });

  Mask const mask(loader);

  ASSERT_EQ(ascendingImageSize, mask.size());
  ASSERT_EQ(ascendingImageResolution, mask.resolution);

  auto const map = mask.map();

  constexpr double d = 255.0 / 7999.0;
  auto i = 0;
  for (Size z = 0; z < mask.size()[2]; ++z) {
    for (Size y = 0; y < mask.size()[1]; ++y) {
      for (Size x = 0; x < mask.size()[0]; ++x) {
        auto const v = map[Size3(x, y, z)];
        auto const refd =
            -128 +
            d * (static_cast<double>(y) + static_cast<double>(x) * 40.0 +
                 static_cast<double>(z) * 800.0);
        auto const ref = static_cast<decltype(v)>(std::round(refd));

        ASSERT_EQ(ref, v);
        ++i;
      }
    }
  }
}

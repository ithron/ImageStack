/// @file testImageStack.cpp
/// @brief File contains unit tests for ImageStack

#include <ImageStack/ImageStack.h>

#include <gtest/gtest.h>

#include <algorithm>
#include <iostream>

#pragma clang diagnostic ignored "-Wglobal-constructors"
#pragma clang diagnostic ignored "-Wmissing-variable-declarations"
#pragma clang diagnostic ignored "-Wused-but-marked-unused"
#pragma clang diagnostic ignored "-Wcovered-switch-default"

using namespace ImageStack;

template <class D, class S> struct ValueCast {
  constexpr auto operator()(S const &s) const noexcept {
    return static_cast<D const &>(s);
  }
};

template <class DT, class S> struct ValueCast<std::array<DT, 4>, S> {
  constexpr auto operator()(S const &s) const noexcept {
    return std::array<DT, 4>{{static_cast<DT>(s), static_cast<DT>(s),
                              static_cast<DT>(s), static_cast<DT>(s)}};
  }
};

template <class D, class S> constexpr auto valueCast(S const &s) noexcept {
  return ValueCast<D, S>{}(s);
}

/// @brief dummy loader that fills the image with values
class DummyLoader {
public:
  constexpr auto size() const { return std::array<Size, 3>{{42, 23, 5}}; }

  template <class T, class OutIter> void readData(OutIter out) const {
    for (auto const &v : storedValues<T>()) *out++ = v;
  }

  template <class T> auto storedValues() const {
    auto const n = indexProduct(size());
    std::vector<T> values;
    int i = 0;
    std::generate_n(std::back_inserter(values), n * 3 / 5,
                    [&i]() { return valueCast<T>(i++); });
    i = 0;
    std::generate_n(std::back_inserter(values), n * 2 / 5,
                    [&i]() { return valueCast<T>(i++); });
    return values;
  }

  template <class T> auto uniqueValues() const {
    std::vector<T> values;
    auto const n = indexProduct(size()) * 3 / 5;
    int i = 0;
    std::generate_n(std::back_inserter(values), n,
                    [&] { return valueCast<T>(i++); });
    return values;
  }
};

/// @brief Test Fixture for ImageStack tests
template <class T> class ImageStackTest : public ::testing::Test {
public:
  using IS = T;
};

/// @brief ImageStack<> types to run tests with
using ImageStackTypes = ::testing::Types<
    ::ImageStack::ImageStack<std::int32_t, HostStorage>,
    ::ImageStack::ImageStack<std::array<std::uint8_t, 4>, HostStorage>,
    ::ImageStack::ImageStack<float, HostStorage>>;

TYPED_TEST_CASE(ImageStackTest, ImageStackTypes);

/// Creates an empty ImageStack<> instance and tests if
///   - `empty() == true`
///   - `size() == 0`
///   - `numSlices() == 0`
///   - `uniqueValues()` is empty
///   - @c map() terminates the program
TYPED_TEST(ImageStackTest, CreateEmpty) {
  using IS = typename TestFixture::IS;

  IS const is;

  ASSERT_TRUE(is.empty());
  ASSERT_EQ(Size3(0, 0, 0), is.size());
  ASSERT_EQ(0, is.numSlices());
  ASSERT_TRUE(is.uniqueValues().empty());
  
  ::testing::FLAGS_gtest_death_test_style = "threadsafe";
#ifndef XCODE_BUILD // Death tests somehow doe not work with xcode test
  ASSERT_DEATH(is.map(), "");
#endif
}

/// Creates an ImageStack<> instance by using DummyLoader and test if
///   - `empty() == false`
///   - `size() == DummyLoader{}.size()`
///   - `numSlices() == DummyLoader{}.size()[2]`
///   - `uniqueValues()` matches the loaded unique values
///   - the stored values are the right ones
TYPED_TEST(ImageStackTest, CreateWithLoader) {
  using IS = typename TestFixture::IS;
  using T = typename IS::StorageType;

  IS const img(DummyLoader{});

  ASSERT_FALSE(img.empty());
  ASSERT_TRUE(indexEqual(DummyLoader{}.size(), img.size()));
  ASSERT_EQ(DummyLoader{}.size()[2], img.numSlices());

  auto const uniqueValues = img.uniqueValues();
  ASSERT_TRUE(std::equal(uniqueValues.cbegin(), uniqueValues.cend(),
                         DummyLoader{}.uniqueValues<T>().cbegin()));

  auto const map = img.map();
  ASSERT_TRUE(std::equal(map.cbegin(), map.cend(),
                         DummyLoader{}.storedValues<T>().cbegin()));
}

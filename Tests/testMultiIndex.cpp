/// @file testMultiIndex.cpp
/// @brief File contains unit tests for multi index related functions

#include <ImageStack/MultiIndex.h>

#include <Eigen/Core>
#include <gtest/gtest.h>

#include <array>

#pragma clang diagnostic ignored "-Wglobal-constructors"

using namespace ImageStack;

using AI1 = std::array<int, 1>;
using AI2 = std::array<int, 2>;
using AI3 = std::array<int, 3>;
using AI4 = std::array<int, 4>;

using MI1 = Eigen::Matrix<int, 1, 1>;
using MI2 = Eigen::Matrix<int, 2, 1>;
using MI3 = Eigen::Matrix<int, 3, 1>;
using MI4 = Eigen::Matrix<int, 4, 1>;

static_assert(dims_v<AI1> == 1, "Error in dims_v implementation");
static_assert(dims_v<AI2> == 2, "Error in dims_v implementation");
static_assert(dims_v<AI3> == 3, "Error in dims_v implementation");
static_assert(dims_v<AI4> == 4, "Error in dims_v implementation");

static_assert(dims_v<MI1> == 1, "Error in dims_v implementation");
static_assert(dims_v<MI2> == 2, "Error in dims_v implementation");
// The two tests below fail on clang-3.5, but the two test above succeed.
// Since no workaround could be found, 3.6 is the minimal supported clang
// version
static_assert(dims_v<MI3> == 3, "Error in dims_v implementation");
static_assert(dims_v<MI4> == 4, "Error in dims_v implementation");

static_assert(isModelOfMultiIndex_v<decltype(MI1::Zero())>,
              "Eigen constant Zero not detected as a model of multi index");
static_assert(dims_v<decltype(MI1::Zero())> == 1,
              "dims_v<> not working correctly for Eigen Zero type");

/// @brief Check if toLinear produces the right result for 1,2,3,4 dimensions
TEST(MultiIndex, toLinear) {
  AI1 constexpr i1{{3}};
  AI2 constexpr i2{{3, 5}};
  AI3 constexpr i3{{3, 5, 8}};
  AI4 constexpr i4{{3, 5, 8, 11}};

  AI4 constexpr s{{11, 12, 13, 14}};

  ASSERT_EQ(3, toLinear(i1, s));
  ASSERT_EQ(58, toLinear(i2, s));
  ASSERT_EQ(1114, toLinear(i3, s));
  ASSERT_EQ(19990, toLinear(i4, s));
  ASSERT_EQ(23, toLinear(23, s));

  // Test case from issue #1 (https://github.com/ithron/ImageStack/issues/1)
  ASSERT_EQ(12, toLinear(AI3{{0, 0, 1}}, AI3{{3, 4, 5}}));
}

/// @brief Check if toLinear produces the right result for 1,2,3,4 dimensions
TEST(MultiIndex, toLinearReorder) {
  AI1 constexpr i1{{3}};
  AI2 constexpr i2{{3, 5}};
  AI3 constexpr i3{{3, 5, 8}};
  AI4 constexpr i4{{3, 5, 8, 11}};

  AI4 constexpr s{{11, 12, 13, 14}};

  auto constexpr order1 = std::index_sequence<0>{};
  auto constexpr order2 = std::index_sequence<1, 0>{};
  auto constexpr order3 = std::index_sequence<2, 0, 1>{};
  auto constexpr order4 = std::index_sequence<3, 2, 0, 1>{};

  ASSERT_EQ(3, toLinearReorder(i1, s, order1));
  ASSERT_EQ(41, toLinearReorder(i2, s, order2));
  ASSERT_EQ(762, toLinearReorder(i3, s, order3));
  ASSERT_EQ(10679, toLinearReorder(i4, s, order4));
}

/// @brief Check if subindex return the right result
TEST(MultiIndex, subindex) {
  AI1 constexpr i1{{3}};
  AI4 constexpr i4{{3, 5, 8, 11}};

  static_assert(dims(subindex<0>(i1)) == 0,
                "A 0 dimensional index should be empty");
  ASSERT_EQ(0, dims(subindex<0>(i1)));

  ASSERT_EQ(3, subindex<1>(i1)[0]);

  ASSERT_EQ(2, dims(subindex<2, 1, 3>(i4)));
  ASSERT_EQ(5, (subindex<2, 1, 3>(i4)[0]));
  ASSERT_EQ(11, (subindex<2, 1, 3>(i4)[1]));
}

/// @brief Test indexSum for 1,2,3 dimensions
TEST(MultiIndex, indexSum) {
  AI1 constexpr i1{{23}};
  AI2 constexpr i2{{5, 23}};
  AI3 constexpr i3{{5, 23, 42}};

  ASSERT_EQ(23, indexSum(i1));
  ASSERT_EQ(28, indexSum(i2));
  ASSERT_EQ(70, indexSum(i3));
}

/// @brief Test indexProduct for 1,2,3 dimensions
TEST(MultiIndex, indexProduct) {
  AI1 constexpr i1{{23}};
  AI2 constexpr i2{{5, 23}};
  AI3 constexpr i3{{5, 23, 42}};
  AI3 constexpr i32{{5, 0, 42}};

  ASSERT_EQ(23, indexProduct(i1));
  ASSERT_EQ(115, indexProduct(i2));
  ASSERT_EQ(4830, indexProduct(i3));
  ASSERT_EQ(0, indexProduct(i32));
}

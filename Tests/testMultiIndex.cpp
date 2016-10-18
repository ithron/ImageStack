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
static_assert(dims_v<MI3> == 3, "Error in dims_v implementation");
static_assert(dims_v<MI4> == 4, "Error in dims_v implementation");

TEST(MultiIndex, toLinear) {
  AI1 constexpr i1{{3}};
  AI2 constexpr i2{{3, 5}};
  AI3 constexpr i3{{3, 5, 8}};
  AI4 constexpr i4{{3, 5, 8, 11}};

  AI4 constexpr s{{11, 12, 13, 14}};

  ASSERT_EQ(3, toLinear(i1, s));
  ASSERT_EQ(58, toLinear(i2, s));
  ASSERT_EQ(154, toLinear(i3, s));
  ASSERT_EQ(297, toLinear(i4, s));
}

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
  ASSERT_EQ(102, toLinearReorder(i3, s, order3));
  ASSERT_EQ(217, toLinearReorder(i4, s, order4));
}


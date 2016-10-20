/// @file testMappedMemory.cpp
/// @brief Unit tests for MappedMemory types
#include <ImageStack/MappedMemory.h>

#include <gtest/gtest.h>

#pragma clang diagnostic ignored "-Wglobal-constructors"

using namespace ImageStack;

using I1 = std::array<Size, 1>;
using I2 = std::array<Size, 2>;
using I3 = std::array<Size, 3>;
using I4 = std::array<Size, 4>;

using MM1 = MappedHostMemory<int, 1>;
using MM2 = MappedHostMemory<int, 2>;
using MM3 = MappedHostMemory<int, 3>;
using MM4 = MappedHostMemory<int, 4>;

constexpr std::array<int, 360> data{
    {0,   1,   2,   3,   4,   5,   6,   7,   8,   9,   10,  11,  12,  13,
     14,  15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,
     28,  29,  30,  31,  32,  33,  34,  35,  36,  37,  38,  39,  40,  41,
     42,  43,  44,  45,  46,  47,  48,  49,  50,  51,  52,  53,  54,  55,
     56,  57,  58,  59,  60,  61,  62,  63,  64,  65,  66,  67,  68,  69,
     70,  71,  72,  73,  74,  75,  76,  77,  78,  79,  80,  81,  82,  83,
     84,  85,  86,  87,  88,  89,  90,  91,  92,  93,  94,  95,  96,  97,
     98,  99,  100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111,
     112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125,
     126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139,
     140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153,
     154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167,
     168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 180, 181,
     182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194, 195,
     196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209,
     210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223,
     224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237,
     238, 239, 240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251,
     252, 253, 254, 255, 256, 257, 258, 259, 260, 261, 262, 263, 264, 265,
     266, 267, 268, 269, 270, 271, 272, 273, 274, 275, 276, 277, 278, 279,
     280, 281, 282, 283, 284, 285, 286, 287, 288, 289, 290, 291, 292, 293,
     294, 295, 296, 297, 298, 299, 300, 301, 302, 303, 304, 305, 306, 307,
     308, 309, 310, 311, 312, 313, 314, 315, 316, 317, 318, 319, 320, 321,
     322, 323, 324, 325, 326, 327, 328, 329, 330, 331, 332, 333, 334, 335,
     336, 337, 338, 339, 340, 341, 342, 343, 344, 345, 346, 347, 348, 349,
     350, 351, 352, 353, 354, 355, 356, 357, 358, 359}};

/// @brief Test MappedHostMemory with empty memory regions for 1, 2, 3, 4
/// dimensions
///
/// The following tests are performed for each dimensions:
///   - `begin() == end()`
///   - @c size() has the right number of dimensions
///   - @c size() is 0 in every dimension
///   - `linearSize() == 0`
TEST(MappedHostMemory, Empty) {
  constexpr I3 s{{0, 0, 0}};
  int dummy{0};

  MM1 const mm1{&dummy, s};

  ASSERT_EQ(mm1.begin(), mm1.end());
  ASSERT_EQ(1, mm1.size().size());
  ASSERT_TRUE((std::array<Size, 1>{{0}} == mm1.size()));
  ASSERT_EQ(0, mm1.linearSize());

  MM2 const mm2{&dummy, s};

  ASSERT_EQ(mm2.begin(), mm2.end());
  ASSERT_EQ(2, mm2.size().size());
  ASSERT_TRUE((std::array<Size, 2>{{0, 0}} == mm2.size()));
  ASSERT_EQ(0, mm2.linearSize());

  MM3 const mm3{&dummy, s};

  ASSERT_EQ(mm3.begin(), mm3.end());
  ASSERT_EQ(3, mm3.size().size());
  ASSERT_TRUE((std::array<Size, 3>{{0, 0, 0}} == mm3.size()));
  ASSERT_EQ(0, mm3.linearSize());

  MM4 const mm4{&dummy, s};

  ASSERT_EQ(mm4.begin(), mm4.end());
  ASSERT_EQ(4, mm4.size().size());
  ASSERT_TRUE((std::array<Size, 4>{{0, 0, 0, 0}} == mm4.size()));
  ASSERT_EQ(0, mm4.linearSize());
}

/// Test that 1D MappedHostMemory represents the mapped memory region
TEST(MappedHostMemory, Continuous1D) {
  constexpr I1 s{{360}};
  MM1 const m{const_cast<int *>(data.data()), s};

  ASSERT_TRUE(std::equal(m.begin(), m.end(), data.begin(), data.end()));
  ASSERT_EQ(data.size(), std::distance(m.begin(), m.end()));

  for (Size x = 0; x < s[0]; ++x) ASSERT_EQ(data[x], m[I1{{x}}]);
}

/// Test that 2D MappedHostMemory represents the mapped memory region
TEST(MappedHostMemory, Continuous2D) {
  constexpr I4 s{{3, 4, 5, 6}};
  MM2 const m{const_cast<int *>(data.data()), s};

  ASSERT_EQ(3 * 4, std::distance(m.begin(), m.end()));
  ASSERT_TRUE(
      std::equal(m.begin(), m.end(), data.begin(), data.begin() + 3 * 4));

  Size i{0};
  for (Size y = 0; y < s[1]; ++y) {
    for (Size x = 0; x < s[0]; ++x) {
      ASSERT_EQ(data[i], (m[I2{{x, y}}]));
      ++i;
    }
  }
}

/// Test that 3D MappedHostMemory represents the mapped memory region
TEST(MappedHostMemory, Continuous3D) {
  constexpr I4 s{{3, 4, 5, 6}};
  MM3 const m{const_cast<int *>(data.data()), s};

  ASSERT_EQ(3 * 4 * 5, std::distance(m.begin(), m.end()));
  ASSERT_TRUE(
      std::equal(m.begin(), m.end(), data.begin(), data.begin() + 3 * 4 * 5));

  Size i{0};
  for (Size z = 0; z < s[2]; ++z) {
    for (Size y = 0; y < s[1]; ++y) {
      for (Size x = 0; x < s[0]; ++x) {
        I3 const idx{{x, y, z}};
        ASSERT_EQ(data[i], (m[idx]));
        ++i;
      }
    }
  }
}

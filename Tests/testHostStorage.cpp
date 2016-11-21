/// @file testHostStorage.cpp
/// @brief Contains unit tests for HostStorage class

#pragma clang diagnostic ignored "-Wglobal-constructors"
#pragma clang diagnostic ignored "-Wused-but-marked-unused"
#pragma clang diagnostic ignored "-Wcovered-switch-default"

#include <ImageStack/HostStorage.h>

#include <gtest/gtest.h>

#include <random>

using namespace ImageStack;

using HS = HostStorage<int>;

/// Creates an empty HostStorage<int> object and tests if
///   - `empty() == true`
///   - `linearSize() == 0`
///   - `size() == 0`
///   - `map()` terminates the program (violated precondition)
TEST(HostStorage, CreateEmpty) {
  HS store(Size3::Zero());

  ASSERT_TRUE(store.empty());
  ASSERT_EQ(0, store.linearSize());
  ASSERT_EQ(Size3::Zero(), store.size());

  ::testing::FLAGS_gtest_death_test_style = "threadsafe";
#ifndef XCODE_BUILD // Death tests somehow doe not work with xcode test
  ASSERT_DEATH({ store.map(); }, "");
#endif
}

/// Creates an uninitialized HostStorage<int> object and tests if
///   - `empty() == false`
///   - @c size() returns the correct value
///   - @c linearSize() returns the correct value
TEST(HostStorage, CreateUninitialized) {
  HS const store(Size3(23, 5, 42));

  ASSERT_FALSE(store.empty());
  ASSERT_EQ(Size3(23, 5, 42), store.size());
  ASSERT_EQ(4830, store.linearSize());
}

/// Creates an initialized (with a single value) HostStorage<int> object and
/// tests if
///   - `empty() == false`
///   - @c size() returns the correct value
///   - @c linearSize() returns the correct value
///   - `store.map[i] == init` for all @c i, when @c c is the initialization
///     value
TEST(HostStorage, CreateConstantInitialized) {
  HS const store(Size3(23, 42, 5), 123);

  ASSERT_FALSE(store.empty());
  ASSERT_EQ(Size3(23, 42, 5), store.size());
  ASSERT_EQ(4830, store.linearSize());

  auto const map = store.map();

  for (auto const x : map) ASSERT_EQ(123, x);
}

/// Creates an initialized HostStorage<int> object and tests if
///   - `empty() == false`
///   - @c size() returns the correct value
///   - @c linearSize() returns the correct value
///   - All values of of the storage were correctly initialized
TEST(HostStorage, CreateInitialized) {
  std::vector<int> src(4830);
  std::iota(src.begin(), src.end(), 0);
  HS const store(Size3(23, 42, 5), src);

  ASSERT_FALSE(store.empty());
  ASSERT_EQ(Size3(23, 42, 5), store.size());
  ASSERT_EQ(4830, store.linearSize());

  auto const map = store.map();

  ASSERT_TRUE(std::equal(map.begin(), map.end(), src.begin()));
}

/// Creates an uninitialized HostSTorage<int> object @c s and fills @c s with
/// random data. Then test if the data in @c s matches the source data.
TEST(HostStorage, Modify) {
  // generate test data
  std::vector<int> src;
  std::random_device rd;
  std::generate_n(std::back_inserter(src), 4830, [&rd]() { return rd(); });
  // Create destination storage
  HS store(Size3(5, 23, 42));

  std::copy(src.begin(), src.end(), store.map().begin());

  ASSERT_TRUE(std::equal(src.begin(), src.end(), store.map().begin()));
}

/// Creates an uninitialized HostStorage<int> object @c s and fills @c s with
/// random data. Then create another HostStorage<int> obejct @c t by copying
/// @c s.
TEST(HostStorage, CopyConstruct) {
  // generate test data
  std::vector<int> src;
  std::random_device rd;
  std::generate_n(std::back_inserter(src), 4830, [&rd]() { return rd(); });
  // Create destination storage
  HS store(Size3(5, 23, 42));
  
  std::copy(src.begin(), src.end(), store.map().begin());
  
  HS const cpy(store);
  
  auto const srcMap = store.map();
  auto const dstMap = cpy.map();
  
  ASSERT_TRUE(std::equal(srcMap.cbegin(), srcMap.cend(), dstMap.begin()));
}

/// Creates an uninitialized HostStorage<int> object @c s and fills @c s with
/// random data. Then create another HostStorage<int> obejct @c t by moving
/// @c s.
TEST(HostStorage, MoveConstruct) {
  // generate test data
  std::vector<int> src;
  std::random_device rd;
  std::generate_n(std::back_inserter(src), 4830, [&rd]() { return rd(); });
  // Create destination storage
  HS store(Size3(5, 23, 42));
  
  std::copy(src.begin(), src.end(), store.map().begin());
  
  HS const mve(std::move(store));
  
  auto const dstMap = mve.map();
  
  ASSERT_TRUE(std::equal(src.cbegin(), src.cend(), dstMap.begin()));
}

/// Creates an uninitialized HostStorage<int> object @c s and fills @c s with
/// random data. Then create another HostStorage<int> obejct @c t and copy @c s
/// to @c t.
TEST(HostStorage, Copy) {
  // generate test data
  std::vector<int> src;
  std::random_device rd;
  std::generate_n(std::back_inserter(src), 4830, [&rd]() { return rd(); });
  // Create destination storage
  HS store(Size3(5, 23, 42));
  
  std::copy(src.begin(), src.end(), store.map().begin());
  
  HS cpy(Size3::Zero());
  cpy = store;
  
  auto const srcMap = store.map();
  auto const dstMap = cpy.map();
  
  ASSERT_TRUE(std::equal(srcMap.cbegin(), srcMap.cend(), dstMap.begin()));
}

/// Creates an uninitialized HostStorage<int> object @c s and fills @c s with
/// random data. Then create another HostStorage<int> obejct @c t and then move
/// @c s to @c t.
TEST(HostStorage, Move) {
  // generate test data
  std::vector<int> src;
  std::random_device rd;
  std::generate_n(std::back_inserter(src), 4830, [&rd]() { return rd(); });
  // Create destination storage
  HS store(Size3(5, 23, 42));
  
  std::copy(src.begin(), src.end(), store.map().begin());
  
  HS mve(Size3::Zero());
  mve = std::move(store);
  
  auto const dstMap = mve.map();
  
  ASSERT_TRUE(std::equal(src.cbegin(), src.cend(), dstMap.begin()));
}

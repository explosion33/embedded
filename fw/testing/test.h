#pragma once
#include <functional>
#include <string>

#include "fw/testing/test_suite.h"

namespace fw::testing {
namespace internal {

// Defines the data needed to run a test.
struct Test {
  std::string name;
  std::function<bool(TestSuite::Logger)> fn;
};

// Gets a vector of all registered tests.
std::vector<Test*>& GetTests();

// Helper struct to register a test. Done at construction so we can statically
// register.
struct TestRegisterer {
  TestRegisterer(Test* test) { GetTests().push_back(test); }
};

}  // namespace internal

#define TEST(name)                                              \
  bool name(fw::testing::TestSuite::Logger logger);             \
  static fw::testing::internal::TestRegisterer register_##name( \
      new fw::testing::internal::Test{#name, &name});           \
  bool name(fw::testing::TestSuite::Logger logger)

#define LOG() logger()

}  // namespace fw::testing
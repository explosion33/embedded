#pragma once
#include <functional>
#include <string>

#include "fw/log/failure_stream.h"
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

// Helper to both log a failure and then return. = binds more loosly than << so
// the entire stream is joined and passed.
struct ReturnFailure {
  bool operator=(const fw::log::FailureStream& stream) {
    TestSuite::Log(stream.Text());
    return false;
  }
};

}  // namespace internal

#define TEST(name)                                              \
  bool name(fw::testing::TestSuite::Logger logger);             \
  static fw::testing::internal::TestRegisterer register_##name( \
      new fw::testing::internal::Test{#name, &name});           \
  bool name(fw::testing::TestSuite::Logger logger)

#define LOG() logger()

#define ASSERT_TRUE(cond)                             \
  switch (0)                                          \
  case 0:                                             \
  default:                                            \
    if (cond) {                                       \
    } else                                            \
      return fw::testing::internal::ReturnFailure() = \
                 fw::log::FailureStream(__FILE__, __LINE__, #cond)

#define ASSERT_FALSE(cond) ASSERT_TRUE(!cond)
#define ASSERT_EQ(first, second) ASSERT_TRUE(first == second)
#define ASSERT_NE(first, second) ASSERT_TRUE(first != second)

}  // namespace fw::testing
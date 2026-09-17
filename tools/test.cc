// A simple test to validate C++ bazel builds.
// TODO: Remove once our first C++ test exists.

#include "absl/status/status.h"
#include "gtest/gtest.h"

namespace tools {
namespace {

// Simple function for testing absl status imports. Returns an error if
// `do_error` is true.
absl::Status TestFunction(bool do_error) {
  if (do_error) {
    return absl::InvalidArgumentError("error");
  }
  return absl::OkStatus();
}

TEST(Test, True) {
  EXPECT_TRUE(TestFunction(false).ok());
  EXPECT_FALSE(TestFunction(true).ok());
}

}  // namespace
}  // namespace tools

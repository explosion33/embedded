#include "fw/testing/test.h"

#include "fw/os/main.h"

namespace fw::testing {
namespace {}

namespace internal {
std::vector<Test*>& GetTests() {
  static std::vector<Test*> tests;
  return tests;
}

}  // namespace internal
}  // namespace fw::testing

void fw::os::AppMain() {
  testing::TestSuite suite;
  for (fw::testing::internal::Test* test : fw::testing::internal::GetTests()) {
    suite.RunTest(test->name, test->fn);
  }
}
#pragma once
#include <functional>
#include <string_view>

namespace fw::testing {

// Runs and records a series of individual tests within one binary.
// TODO: Add support for parallel tests.
class TestSuite {
 public:
  TestSuite() = default;
  ~TestSuite();

  using LogFunction = std::function<void(std::string_view msg)>;

  // Runs the provided test, a test returns true on pass false if not, and gets
  // an injected logging function. Returns the test status.
  bool RunTest(std::string_view name, std::function<bool(LogFunction)> test);

  // Logs a message on the test harness.
  static void Log(std::string_view msg);

 private:
  // True if at least one test has ran on the suite.
  bool ran_ = false;
};

}  // namespace fw::testing
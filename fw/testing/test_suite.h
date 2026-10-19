#pragma once
#include <functional>
#include <string_view>

#include "fw/log/invocable_stream.h"
#include "fw/testing/test_suite_interface.h"

namespace fw::testing {

// Runs and records a series of individual tests within one binary.
// TODO: Add support for parallel tests.
class TestSuite : public TestSuiteInterface {
 public:
  TestSuite() = default;
  ~TestSuite();

  // Runs the provided test, a test returns true on pass false if not, and gets
  // an injected logging function. Returns the test status.
  bool RunTest(std::string_view name,
               std::function<void(TestSuiteInterface&)> test);

  // Logs a message on the test harness.
  static void Log(std::string_view msg);

  fw::log::InvolableStream Log() override {
    return fw::log::InvolableStream([](std::string_view msg) { Log(msg); });
  }

  void FailTest() override { failure_ = true; }

 private:
  // True if at least one test has ran on the suite.
  bool ran_ = false;

  // True if the currently executing test has failed.
  bool failure_ = false;
};

}  // namespace fw::testing
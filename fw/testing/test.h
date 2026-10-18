#pragma once
#include <functional>
#include <string_view>

#include "fw/log/invocable_stream.h"

namespace fw::testing {

// Runs and records a series of individual tests within one binary.
// TODO: Add support for parallel tests.
class TestSuite {
 public:
  TestSuite() = default;
  ~TestSuite();

  // Logger for the test suite.
  struct Logger {
    fw::log::InvolableStream operator()() {
      return fw::log::InvolableStream([](std::string_view msg) { Log(msg); });
    }
    void operator()(std::string_view msg) { return Log(msg); }
  };

  // Runs the provided test, a test returns true on pass false if not, and gets
  // an injected logging function. Returns the test status.
  bool RunTest(std::string_view name, std::function<bool(Logger)> test);

 private:
  // Logs a message on the test harness.
  static void Log(std::string_view msg);

  // True if at least one test has ran on the suite.
  bool ran_ = false;
};

}  // namespace fw::testing
#pragma once

#include "fw/log/invocable_stream.h"

namespace fw::testing {

// Defines a minimal interface for a test suite. This is intended to be passed
// to a running test to expose control without allowing recursive tests.
class TestSuiteInterface {
 public:
  // Logs the streamed message to the harness.
  virtual fw::log::InvolableStream Log() = 0;

  // Indicates that the currently running test has failed.
  virtual void FailTest() = 0;
};

}  // namespace fw::testing
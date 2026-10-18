#include <cstdio>

#include "fw/boards/nucleo_f446re/board.h"
#include "fw/os/main.h"
#include "fw/testing/test.h"

namespace {
using fw::testing::TestSuite;
}

void fw::os::AppMain() {
  TestSuite suite;
  suite.RunTest("test", [](TestSuite::LogFunction log) {
    log("This test should pass");
    return true;
  });

  suite.RunTest("pass_test_test_2", [](TestSuite::LogFunction log) {
    log("log 1");
    log("log 2");
    log("log 3");
    return true;
  });
}

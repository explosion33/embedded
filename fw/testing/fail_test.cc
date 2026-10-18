#include <cstdio>

#include "fw/boards/nucleo_f446re/board.h"
#include "fw/os/main.h"
#include "fw/testing/test_suite.h"

namespace {
using fw::testing::TestSuite;
}

void fw::os::AppMain() {
  TestSuite suite;
  suite.RunTest("fail_test", [](TestSuite::Logger logger) {
    logger("This test should fail");
    return false;
  });

  suite.RunTest("fail_test_2", [](TestSuite::Logger logger) {
    logger("This test should also fail");
    return false;
  });
}

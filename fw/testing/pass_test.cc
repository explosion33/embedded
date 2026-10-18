#include <cstdio>

#include "fw/boards/nucleo_f446re/board.h"
#include "fw/os/main.h"
#include "fw/testing/test.h"

namespace {
using fw::testing::TestSuite;
}

void fw::os::AppMain() {
  TestSuite suite;
  suite.RunTest("test", [](TestSuite::Logger logger) {
    logger("This test should pass");
    return true;
  });

  suite.RunTest("pass_test_test_2", [](TestSuite::Logger logger) {
    logger("log 1");
    logger("log 2");
    logger() << "streamed " << "log " << 3;
    return true;
  });
}

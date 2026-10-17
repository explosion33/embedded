#include <cstdio>

#include "fw/boards/nucleo_f446re/board.h"
#include "fw/os/main.h"
#include "fw/testing/test.h"

void fw::os::AppMain() {
  fw::testing::LogTest("This test should pass");
  fw::testing::TestPass();
}

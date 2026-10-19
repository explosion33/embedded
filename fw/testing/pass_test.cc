#include <cstdio>
#include <string_view>

#include "fw/boards/nucleo_f446re/board.h"
#include "fw/log/stream.h"
#include "fw/testing/test.h"

namespace {

TEST(test) {
  LOG() << "This test should pass";
  return true;
}

TEST(pass_test_test_2) {
  LOG() << "log 1";
  LOG() << "log 2";
  LOG() << "streamed " << "log " << 3;
  return true;
}

}  // namespace

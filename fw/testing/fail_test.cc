#include <cstdio>

#include "fw/boards/nucleo_f446re/board.h"
#include "fw/testing/test.h"

namespace {
using fw::testing::TestSuite;
}

TEST(fail_test) { ASSERT_TRUE(false) << "This test should fail."; }
TEST(fail_test_2) { ASSERT_TRUE(false) << "This test should also fail."; }

#include <cstdio>

#include "fw/boards/nucleo_f446re/board.h"
#include "fw/testing/test.h"

namespace {
using fw::testing::TestSuite;
}

TEST(fail_true) { ASSERT_TRUE(false) << "This test should fail."; }
TEST(fail_false) { ASSERT_FALSE(true) << "This test should also fail."; }
TEST(fail_eq) { ASSERT_EQ(1, 3); }
TEST(fail_ne) { ASSERT_NE(1, 1); }

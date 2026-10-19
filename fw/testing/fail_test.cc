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
TEST(fail_lt) { ASSERT_LT(2, 2); }
TEST(fail_le) { ASSERT_LE(3, 2); }
TEST(fail_gt) { ASSERT_GT(4, 4); }
TEST(fail_ge) { ASSERT_GE(3, 4); }
TEST(fail_expect) {
  EXPECT_TRUE(false);
  EXPECT_EQ(1, 3);
  EXPECT_NE(1, 1);
  EXPECT_LT(2, 2);
  EXPECT_LE(3, 2);
  EXPECT_GT(4, 4);
  EXPECT_GE(3, 4);
  LOG() << "Test Continues";
}

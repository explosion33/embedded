#include "fw/os/thread.h"

#include "fw/os/time.h"
#include "gtest/gtest.h"

namespace fw::os {
namespace {

TEST(ThreadTest, ThreadExecutesWithArguments) {
  Thread thread;

  bool result = false;
  thread.Start([](bool* result, bool val) { *result = val; }, &result, true);
  thread.Join();
  EXPECT_TRUE(result);
}

TEST(ThreadTest, ThreadJoins) {
  Thread thread;

  bool result = false;
  thread.Start([&result]() {
    Delay(100);
    result = true;
  });

  // Thread should join and we should be able to modify thread state again.
  thread.Join();
  thread.SetPriority(3);
  thread.SetStackDepth(100);

  EXPECT_TRUE(result);
}

TEST(ThreadTest, TwoThreadTest) {
  constexpr size_t kCount = 50;
  size_t count1 = 0;
  size_t count2 = 0;
  bool count2_intermediate = false;

  // Thread 1 should be able to observe progress on thread2, if not they aren't
  // truly running in parallel.
  Thread t1;
  Thread t2;
  t1.Start([&count1, &count2, &count2_intermediate]() {
    while (count1 < kCount) {
      count1++;

      if (!count2_intermediate && count2 > 0 && count2 < kCount) {
        count2_intermediate = true;
      }
      Delay(1);
    }
  });
  t2.Start([&count2]() {
    while (count2 < kCount) {
      count2++;
      Delay(1);
    }
  });

  t1.Join();
  t2.Join();

  EXPECT_TRUE(count2_intermediate);
  EXPECT_EQ(count1, kCount);
  EXPECT_EQ(count2, kCount);
}

}  // namespace
}  // namespace fw::os
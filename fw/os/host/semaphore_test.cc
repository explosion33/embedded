#include "fw/os/semaphore.h"

#include "fw/os/thread.h"
#include "fw/os/time.h"
#include "gtest/gtest.h"

namespace fw::os {
namespace {

TEST(SemaphoreTest, TestIncrementDecrement) {
  for (size_t max_count : {1, 5, 20}) {
    Semaphore sem(max_count);
    Thread thread;

    size_t count = 0;
    bool run = true;
    thread.Start([&count, &run, &sem]() {
      while (run) {
        sem.Take();
        count++;
      }
    });

    Delay(10);
    EXPECT_EQ(count, 0);

    EXPECT_TRUE(sem.Give());
    Delay(10);
    EXPECT_EQ(count, 1);
    count = 0;

    for (size_t i = 0; i < max_count; i++) {
      EXPECT_TRUE(sem.Give());
    }
    EXPECT_EQ(sem.Count(), max_count);
    Delay(10);
    EXPECT_EQ(count, max_count);

    run = false;
    CHECK(sem.Give());
    thread.Join();

    sem.Give();
    EXPECT_TRUE(sem.TryTakeFor(10));
    EXPECT_FALSE(sem.TryTakeFor(10));
  }
}

}  // namespace
}  // namespace fw::os
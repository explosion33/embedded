#include "fw/os/mutex.h"

#include "fw/os/thread.h"
#include "fw/os/time.h"
#include "gtest/gtest.h"

namespace fw::os {
namespace {

TEST(MutexTest, BlockingMutex) {
  Mutex mutex;

  bool run = true;
  size_t count = 0;
  Thread thread;
  thread.Start([&mutex, &count, &run]() {
    while (run) {
      mutex.Lock();
      count++;
      mutex.Unlock();
      Delay(1);
    }
  });

  // The thread should be able to lock / unlock the mutex and increment the
  // count.
  Delay(20);
  EXPECT_NE(count, 0);

  // Locking the mutex should stop the count from incrementing.
  mutex.Lock();
  count = 0;
  Delay(20);
  EXPECT_EQ(count, 0);

  // Unlocking should re-allow the thread to progress.
  mutex.Unlock();
  Delay(20);
  EXPECT_NE(count, 0);

  run = false;
  thread.Join();
}

TEST(MutexTest, MutexTimeout) {
  constexpr size_t kTimeout = 10;
  constexpr size_t kError = 1;
  Mutex mutex;

  // Locking the mutex should cause TryLockFor to timeout.
  mutex.Lock();
  size_t start = Millis();
  EXPECT_FALSE(mutex.TryLockFor(kTimeout));

  // We expect the elapsed time to be ~10ms.
  const size_t elapsed = Millis() - start;
  const size_t diff =
      elapsed > kTimeout ? elapsed - kTimeout : kTimeout - elapsed;
  EXPECT_LE(diff, kError) << elapsed;

  // Unlocking the mutex should acquire the lock.
  mutex.Unlock();
  EXPECT_TRUE(mutex.TryLockFor(kTimeout));
}

}  // namespace
}  // namespace fw::os
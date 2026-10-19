#include <cstdio>

#include "fw/log/check.h"
#include "fw/os/mutex.h"
#include "fw/os/semaphore.h"
#include "fw/os/thread.h"
#include "fw/os/time.h"
#include "fw/testing/test.h"

namespace fw::os {
namespace {

TEST(TestDelayAndMillis) {
  constexpr size_t kMaxError = 1;

  for (size_t millis : {10, 100, 500, 1000}) {
    LOG() << "Delaying " << millis << "ms.";
    const size_t start = Millis();
    Delay(millis);

    const size_t elapsed = Millis() - start;
    const size_t diff = millis > elapsed ? millis - elapsed : elapsed - millis;
    ASSERT_LE(diff, kMaxError) << millis;
  }
}

TEST(ThreadExecutesWithArguments) {
  Thread thread;

  bool result = false;
  thread.Start([](bool* result, bool val) { *result = val; }, &result, true);
  thread.Join();
  ASSERT_TRUE(result);
}

TEST(ThreadJoins) {
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

  ASSERT_TRUE(result);
}

TEST(TwoThreadTest) {
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

  ASSERT_TRUE(count2_intermediate);
  ASSERT_EQ(count1, kCount);
  ASSERT_EQ(count2, kCount);
}

TEST(ThreadPriority) {
  constexpr size_t kRunTime = 100;
  Thread thread;
  Thread thread2;
  thread.SetPriority(3);

  bool result = true;
  bool t2_start = false;

  // The higher priority task should never yield to the lower priority task
  // while ready. Spin looping Millis() ensures it does not yield unlike Delay
  // would.
  thread.Start([&t2_start, &result]() {
    const size_t start = Millis();
    while (Millis() - start < kRunTime) {
      if (t2_start) {
        result = false;
      }
    }
  });

  // Give the thread a chance to start.
  Delay(5);

  thread2.Start([&t2_start]() { t2_start = true; });

  thread.Join();
  thread2.Join();

  // We expect thread1 to have not seen thread2 start, but thread2 to still
  // finish after thread1 has.
  ASSERT_TRUE(result);
  ASSERT_TRUE(t2_start);
}

TEST(BlockingMutex) {
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
  ASSERT_NE(count, 0);

  // Locking the mutex should stop the count from incrementing.
  mutex.Lock();
  count = 0;
  Delay(20);
  ASSERT_EQ(count, 0);

  // Unlocking should re-allow the thread to progress.
  mutex.Unlock();
  Delay(20);
  ASSERT_NE(count, 0);

  run = false;
  thread.Join();
}

TEST(MutexTimeout) {
  constexpr size_t kTimeout = 10;
  constexpr size_t kError = 1;
  Mutex mutex;

  // Locking the mutex should cause TryLockFor to timeout.
  mutex.Lock();
  size_t start = Millis();
  ASSERT_FALSE(mutex.TryLockFor(kTimeout));

  // We expect the elapsed time to be ~10ms.
  const size_t elapsed = Millis() - start;
  const size_t diff =
      elapsed > kTimeout ? elapsed - kTimeout : kTimeout - elapsed;
  ASSERT_LE(diff, kError) << elapsed;

  // Unlocking the mutex should acquire the lock.
  mutex.Unlock();
  ASSERT_TRUE(mutex.TryLockFor(kTimeout));
}

TEST(SemaphoreTest) {
  for (size_t max_count : {1, 5, 20}) {
    LOG() << "Testing semaphore with max_count: " << max_count;
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
    ASSERT_EQ(count, 0);

    ASSERT_TRUE(sem.Give());
    Delay(10);
    ASSERT_EQ(count, 1);
    count = 0;

    for (size_t i = 0; i < max_count; i++) {
      ASSERT_TRUE(sem.Give());
    }
    ASSERT_EQ(sem.Count(), max_count);
    Delay(10);
    ASSERT_EQ(count, max_count);

    run = false;
    CHECK(sem.Give());
    thread.Join();

    sem.Give();
    ASSERT_TRUE(sem.TryTakeFor(10));
    ASSERT_FALSE(sem.TryTakeFor(10));
  }
}

TEST(ThreadStop) {
  Thread thread;

  size_t count = 0;
  thread.Start([&count]() {
    while (true) {
      Delay(1);
      count++;
    }
  });

  Delay(20);
  ASSERT_GT(count, 0);

  count = 0;
  thread.Stop();
  thread.Join();

  Delay(20);
  ASSERT_EQ(count, 0);
}

TEST(ThreadPause) {
  Thread thread;

  size_t count = 0;
  thread.Start([&count]() {
    while (true) {
      Delay(1);
      count++;
    }
  });

  Delay(20);
  ASSERT_GT(count, 0);

  count = 0;
  thread.Pause();

  Delay(20);
  ASSERT_EQ(count, 0);

  thread.Resume();
  Delay(20);
  ASSERT_GT(count, 0);
}

}  // namespace
}  // namespace fw::os

#include <cstdio>

#include "fw/log/check.h"
#include "fw/os/main.h"
#include "fw/os/mutex.h"
#include "fw/os/semaphore.h"
#include "fw/os/thread.h"
#include "fw/os/time.h"
#include "fw/testing/test.h"

namespace {
using fw::testing::TestSuite;

// Tests that Delay results and an equivelent change in millis.
bool TestDelayAndMillis(TestSuite::Logger logger) {
  constexpr size_t kMaxError = 1;

  for (size_t millis : {10, 100, 500, 1000}) {
    logger() << "Delaying " << millis << "ms.";
    const size_t start = fw::os::Millis();
    fw::os::Delay(millis);

    const size_t elapsed = fw::os::Millis() - start;
    const int diff = millis - elapsed;
    const size_t abs_diff = diff > 0 ? diff : diff * -1;
    if (abs_diff > kMaxError) {
      logger() << "Delay of " << millis << "ms actually slept for " << elapsed
               << "ms.";
      return false;
    }
  }
  return true;
}

// Starts a simple thread that accepts arguments and modifies values. Testing
// that the variadic arguments reach the function.
bool ThreadExecutesWithArguments(TestSuite::Logger logger) {
  fw::os::Thread thread;

  bool result = false;
  thread.Start([](bool* result, bool val) { *result = val; }, &result, true);
  fw::os::Delay(100);
  return result;
}

// Tests that Thread::Join works properly.
bool ThreadJoins(TestSuite::Logger logger) {
  fw::os::Thread thread;

  bool result = false;
  thread.Start([&result]() {
    fw::os::Delay(100);
    result = true;
  });

  // Thread should join and we should be able to modify thread state again.
  thread.Join();
  thread.SetPriority(3);
  thread.SetStackDepth(100);

  return result;
}

// Spawns two threads and checks that the both complete in parallel.
bool TwoThreadTest(TestSuite::Logger logger) {
  constexpr size_t kCount = 50;
  size_t count1 = 0;
  size_t count2 = 0;
  bool count2_intermediate = false;

  // Thread 1 should be able to observe progress on thread2, if not they aren't
  // truly running in parallel.
  fw::os::Thread t1;
  fw::os::Thread t2;
  t1.Start([&count1, &count2, &count2_intermediate]() {
    while (count1 < kCount) {
      count1++;

      if (!count2_intermediate && count2 > 0 && count2 < kCount) {
        count2_intermediate = true;
      }
      fw::os::Delay(1);
    }
  });
  t2.Start([&count2]() {
    while (count2 < kCount) {
      count2++;
      fw::os::Delay(1);
    }
  });

  // TODO: Replace with Thread Joining
  fw::os::Delay(100);

  if (!count2_intermediate) {
    logger(
        "Thread1 did not observe incremental changes in thread2. Threads ran "
        "in sequence.");
    return false;
  }

  return count1 == kCount && count2 == kCount;
}

// Spawns two threads and ensures the higher priority one does not yield to the
// lower priority one.
bool ThreadPriority(TestSuite::Logger logger) {
  constexpr size_t kRunTime = 100;
  fw::os::Thread thread;
  fw::os::Thread thread2;
  thread.SetPriority(3);

  bool result = true;
  bool t2_start = false;

  // The higher priority task should never yield to the lower priority task
  // while ready. Spin looping Millis() ensures it does not yield unlike Delay
  // would.
  thread.Start([&t2_start, &result]() {
    const size_t start = fw::os::Millis();
    while (fw::os::Millis() - start < kRunTime) {
      if (t2_start) {
        result = false;
      }
    }
  });

  // Give the thread a chance to start.
  fw::os::Delay(5);

  thread2.Start([&t2_start]() { t2_start = true; });

  // TODO: Replace with Join once available.
  fw::os::Delay(kRunTime * 2);

  // We expect thread1 to have not seen thread2 start, but thread2 to still
  // finish after thread1 has.
  return result && t2_start;
}

// Tests a blocking mutex (no timeout) between two threads ensuring each waits
// for mutex acquistition.
bool BlockingMutex(TestSuite::Logger logger) {
  fw::os::Mutex mutex;

  bool run = true;
  size_t count = 0;
  fw::os::Thread thread;
  thread.Start([&mutex, &count, &run]() {
    while (run) {
      mutex.Lock();
      count++;
      mutex.Unlock();
      fw::os::Delay(1);
    }
  });

  // The thread should be able to lock / unlock the mutex and increment the
  // count.
  fw::os::Delay(20);
  if (count == 0) {
    logger("Initial thread state did not tick.");
    return false;
  }

  // Locking the mutex should stop the count from incrementing.
  mutex.Lock();
  count = 0;
  fw::os::Delay(20);
  if (count != 0) {
    logger("Thread was not gated on the mutex.");
    return false;
  }

  // Unlocking should re-allow the thread to progress.
  mutex.Unlock();
  fw::os::Delay(20);
  if (count == 0) {
    logger("Unlocking the mutex did not resume thread.");
    return false;
  }

  // TODO: Replace with Join().
  run = false;
  fw::os::Delay(20);

  return true;
}

// Tests the mutex's timeout lock call.
bool MutexTimeout(TestSuite::Logger logger) {
  constexpr size_t kTimeout = 10;
  constexpr size_t kError = 1;
  fw::os::Mutex mutex;

  // Locking the mutex should cause TryLockFor to timeout.
  mutex.Lock();
  size_t start = fw::os::Millis();
  if (mutex.TryLockFor(kTimeout)) {
    logger("TryLockFor locked the mutex while already locked.");
    return false;
  }

  // We expect the elapsed time to be ~10ms.
  const size_t elapsed = fw::os::Millis() - start;
  const size_t diff =
      elapsed > kTimeout ? elapsed - kTimeout : kTimeout - elapsed;

  if (diff > kError) {
    logger() << "Expected TryLockFor to timeout after " << kTimeout
             << "ms, it took " << elapsed << "ms";
    return false;
  }

  // Unlocking the mutex should acquire the lock.
  mutex.Unlock();
  if (!mutex.TryLockFor(kTimeout)) {
    logger("Mutex was not locked.");
    return false;
  }

  return true;
}

// Tests that semaphores work as expected.
bool SemaphoreTest(TestSuite::Logger logger) {
#define EXPECT_COUNT(expected)                                            \
  if (count != expected) {                                                \
    logger() << "Thread consumed " << count << " expected it to consume " \
             << expected;                                                 \
    return false;                                                         \
  }

  for (size_t max_count : {1, 5, 20}) {
    logger() << "Testing semaphore with max_count: " << max_count;
    fw::os::Semaphore sem(max_count);
    fw::os::Thread thread;

    size_t count = 0;
    bool run = true;
    thread.Start([&count, &run, &sem]() {
      while (run) {
        sem.Take();
        count++;
      }
    });

    fw::os::Delay(10);
    EXPECT_COUNT(0);

    if (!sem.Give()) {
      logger("Failed to give semaphore.");
      return false;
    }
    fw::os::Delay(10);
    EXPECT_COUNT(1);
    count = 0;

    for (size_t i = 0; i < max_count; i++) {
      if (!sem.Give()) {
        logger("Failed to give semaphore.");
        return false;
      }
    }
    if (sem.Count() != max_count) {
      logger() << "Expected count of " << max_count << " Got " << sem.Count();
      return false;
    }
    fw::os::Delay(10);
    EXPECT_COUNT(max_count);

    run = false;
    CHECK(sem.Give());
    fw::os::Delay(10);

    sem.Give();
    if (!sem.TryTakeFor(10)) {
      logger() << "Failed to take semaphore.";
      return false;
    }
    if (sem.TryTakeFor(10)) {
      logger() << "Took semaphore when none was available.";
      return false;
    }
  }

#undef EXPECT_COUNT

  return true;
}

}  // namespace

void fw::os::AppMain() {
  TestSuite suite;
  suite.RunTest("TestDelayAndMillis", &TestDelayAndMillis);
  suite.RunTest("ThreadExecutesWithArguments", &ThreadExecutesWithArguments);
  suite.RunTest("ThreadJoins", &ThreadJoins);
  suite.RunTest("TwoThreadTest", &TwoThreadTest);
  suite.RunTest("ThreadPriority", &ThreadPriority);
  suite.RunTest("BlockingMutex", &BlockingMutex);
  suite.RunTest("MutexTimeout", &MutexTimeout);
  suite.RunTest("SemaphoreTest", &SemaphoreTest);
}

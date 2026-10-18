#include <cstdio>

#include "fw/os/main.h"
#include "fw/os/thread.h"
#include "fw/os/time.h"
#include "fw/testing/test.h"

namespace {
using fw::testing::TestSuite;

// Tests that Delay results and an equivelent change in millis.
bool TestDelayAndMillis(TestSuite::LogFunction log) {
  constexpr size_t kMaxError = 5;

  for (size_t millis : {10, 100, 500, 1000}) {
    const size_t start = fw::os::Millis();
    fw::os::Delay(millis);

    const size_t elapsed = fw::os::Millis() - start;
    const int diff = millis - elapsed;
    const size_t abs_diff = diff > 0 ? diff : diff * -1;
    if (abs_diff > kMaxError) {
      // TODO: Add error message once log() supports format strings.
      return false;
    }
  }
  return true;
}

// Starts a simple thread that accepts arguments and modifies values. Testing
// that the variadic arguments reach the function.
bool ThreadExecutesWithArguments(TestSuite::LogFunction log) {
  fw::os::Thread thread;

  bool result = false;
  thread.Start([](bool* result, bool val) { *result = val; }, &result, true);
  fw::os::Delay(100);
  return result;
}

// Spawns two threads and checks that the both complete in parallel.
bool TwoThreadTest(TestSuite::LogFunction log) {
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
    log("Thread1 did not observe incremental changes in thread2. Threads ran "
        "in sequence.");
    return false;
  }

  return count1 == kCount && count2 == kCount;
}

// Spawns two threads and ensures the higher priority one does not yield to the
// lower priority one.
bool ThreadPriority(TestSuite::LogFunction log) {
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

}  // namespace

void fw::os::AppMain() {
  TestSuite suite;
  suite.RunTest("TestDelayAndMillis", &TestDelayAndMillis);
  suite.RunTest("ThreadExecutesWithArguments", &ThreadExecutesWithArguments);
  suite.RunTest("TwoThreadTest", &TwoThreadTest);
  suite.RunTest("ThreadPriority", &ThreadPriority);
}

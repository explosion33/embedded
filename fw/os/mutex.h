#pragma once
#include <cstddef>

namespace fw::os {

// Implements a Mutex using Free RTOS function calls.
class Mutex {
 public:
  Mutex();
  ~Mutex();

  // Locks the mutex.
  void Lock();

  // Tries to lock the mutex for `millis` ms, before timing out. Returns true if
  // the mutex was locked.
  bool TryLockFor(size_t millis);

  // Unlocks the mutex.
  void Unlock();

 private:
  // Type erased mutex handle.
  void* handle_;

  // True if the mutex is currently locked
  bool locked_ = false;
};

}  // namespace fw::os
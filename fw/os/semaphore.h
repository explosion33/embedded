#pragma once
#include <cstddef>

namespace fw::os {

// Implements a Semaphore using FreeRTOS function calls.
class Semaphore {
 public:
  explicit Semaphore(size_t max_count);
  ~Semaphore();

  // Gives to the semaphore. Returns true if succeeded.
  bool Give();

  // Takes from the semaphore, blocking until.
  void Take();

  // Gets the current semaphore count;
  size_t Count();

  // Tries to take a single count for the specified timeout. Returns true if
  // succeeded.
  bool TryTakeFor(size_t millis);

 private:
  // Type erased semaphore handle.
  void* handle_;
};

}  // namespace fw::os
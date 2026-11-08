#include "fw/os/mutex.h"

#include <chrono>
#include <mutex>

#include "fw/log/check.h"

namespace fw::os {

Mutex::Mutex() { handle_ = new std::timed_mutex; }

Mutex::~Mutex() {
  std::timed_mutex* mutex = static_cast<std::timed_mutex*>(handle_);
  if (locked_) {
    Unlock();
  }
  delete mutex;
}

void Mutex::Lock() {
  std::timed_mutex* mutex = static_cast<std::timed_mutex*>(handle_);
  mutex->lock();
  locked_ = true;
}

bool Mutex::TryLockFor(size_t millis) {
  std::timed_mutex* mutex = static_cast<std::timed_mutex*>(handle_);
  const bool res = mutex->try_lock_for(std::chrono::milliseconds(millis));
  if (res) {
    locked_ = true;
  }
  return res;
}

void Mutex::Unlock() {
  CHECK(locked_);
  std::timed_mutex* mutex = static_cast<std::timed_mutex*>(handle_);
  mutex->unlock();
  locked_ = false;
}

}  // namespace fw::os
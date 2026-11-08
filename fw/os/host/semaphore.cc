#include "fw/os/semaphore.h"

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <semaphore>

#include "fw/log/check.h"

namespace fw::os {

namespace {
// Arbitrary max count based on current application.
constexpr size_t kMaxCount = 1000;

// The state tracked by this implementation. Stored in `handle_`.
struct State {
  std::mutex mtx;
  std::condition_variable cv;
  size_t count = 0;
  const size_t max_count;
};

}  // namespace

Semaphore::Semaphore(size_t max_count) {
  CHECK(max_count > 0);
  CHECK(max_count <= kMaxCount);

  handle_ = new State{.max_count = max_count};
}

Semaphore::~Semaphore() { delete static_cast<State*>(handle_); }

bool Semaphore::Give() {
  State* state = static_cast<State*>(handle_);

  {
    std::lock_guard lock(state->mtx);

    if (state->count == state->max_count) {
      return false;
    }
    state->count++;
  }
  state->cv.notify_one();
  return true;
}

void Semaphore::Take() {
  State* state = static_cast<State*>(handle_);
  std::unique_lock lock(state->mtx);
  state->cv.wait(lock, [state] { return state->count > 0; });
  state->count--;
}

bool Semaphore::TryTakeFor(size_t millis) {
  State* state = static_cast<State*>(handle_);
  std::unique_lock lock(state->mtx);
  if (!state->cv.wait_for(lock, std::chrono::milliseconds(millis),
                          [state] { return state->count > 0; })) {
    return false;
  }
  state->count--;
  return true;
}

size_t Semaphore::Count() {
  State* state = static_cast<State*>(handle_);
  return state->count;
}

}  // namespace fw::os
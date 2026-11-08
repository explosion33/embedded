#include "fw/os/semaphore.h"

#include <atomic>
#include <chrono>
#include <semaphore>

#include "fw/log/check.h"

namespace fw::os {

namespace {
// Arbitrary max count based on current application.
constexpr size_t kMaxCount = 1000;

// The state tracked by this implementation. Stored in `handle_`.
struct State {
  std::counting_semaphore<kMaxCount> sem{0};
  std::atomic<size_t> count{0};
  size_t max_count;
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

  // Atomically increment the current count without exceeding the self-tracked
  // max_count.
  size_t curr = state->count.load();
  size_t incremented;
  do {
    if (curr >= state->max_count) {
      return false;
    }
    incremented = curr + 1;
  } while (!state->count.compare_exchange_weak(curr, incremented));
  state->sem.release();
  return true;
}

void Semaphore::Take() {
  State* state = static_cast<State*>(handle_);
  state->sem.acquire();
  state->count.fetch_add(-1);
}

bool Semaphore::TryTakeFor(size_t millis) {
  State* state = static_cast<State*>(handle_);
  bool res = state->sem.try_acquire_for(std::chrono::milliseconds(millis));
  if (res) {
    state->count.fetch_add(-1);
  }
  return res;
}

size_t Semaphore::Count() {
  State* state = static_cast<State*>(handle_);
  return state->count.load();
}

}  // namespace fw::os
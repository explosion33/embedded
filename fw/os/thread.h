#pragma once

#include <cstddef>
#include <optional>
#include <tuple>
#include <utility>

#include "fw/log/check.h"
#include "fw/os/semaphore.h"
#include "fw/os/task_wrapper.h"

namespace fw::os {

// Implements a C++ style thread wrapping FreeRTOS tasks.
class Thread {
 public:
  Thread() : thread_state_sem_(1) { CHECK(thread_state_sem_.Give()); };
  Thread(const Thread&) = delete;
  Thread& operator=(const Thread&) = delete;

  // Sets the stack depth for the thread in bytes.
  void SetStackDepth(size_t depth) {
    CHECK(thread_state_sem_.Count() == 1);
    stack_depth_ = depth;
  }

  // Sets the priority for the given task, 0 being the lowest.
  void SetPriority(size_t priority) {
    CHECK(thread_state_sem_.Count() == 1);
    priority_ = priority;
  }

  // Blocks until the active thread returns, or returns immediately if there was
  // no active task.
  void Join() {
    thread_state_sem_.Take();
    thread_state_sem_.Give();
  }

  // Pauses the thread. Thread must be running.
  void Pause() {
    TryClearTask();
    CHECK(task_.has_value());
    task_->Pause();
  }

  // Resumes a paused thread. Thread must be started and paused.
  void Resume() {
    TryClearTask();
    CHECK(task_.has_value());
    task_->Resume();
  }

  // Stops a thread, once stopped it cannot be restarted. Stopping a thread does
  // not clean up heap allocated memory.
  void Stop() {
    TryClearTask();
    if (task_.has_value()) {
      task_->Stop();
      task_.reset();
      thread_state_sem_.Give();
    }
  }

  // Runs the provided function in the thread with the provided arguments.
  template <typename Fn, typename... Args>
  void Start(Fn fn, Args... args) {
    CHECK(thread_state_sem_.Count() == 1);
    CHECK(thread_state_sem_.TryTakeFor(10));

    // Convert the args and the function call into a single payload struct.
    using TypedPayload = Payload<Fn, Args...>;
    TypedPayload* payload =
        new TypedPayload{std::move(fn), std::tuple<Args...>(std::move(args)...),
                         thread_state_sem_};
    CHECK(payload != nullptr);

    const auto entry = [](void* arg) {
      TypedPayload* heap = static_cast<TypedPayload*>(arg);
      TypedPayload local(std::move(*heap));
      delete heap;

      std::apply(local.fn, std::move(local.args));
      local.sem.Give();
      internal::Task::ExitCurrentTask();
    };

    task_.emplace(internal::Task::Config{
        .fn = entry,
        .arg = payload,
        .name = "",
        .stack_depth_bytes = stack_depth_,
        .priority = priority_,
    });
  }

 private:
  // Holds the dynamic typed thread payload.
  template <typename Fn, typename... Args>
  struct Payload {
    Fn fn;
    std::tuple<Args...> args;
    // Consumed by the thread entry for state tracking.
    fw::os::Semaphore& sem;
  };

  // Checks if the task has finished and clears the local handle.
  void TryClearTask() {
    if (thread_state_sem_.Count() == 1) {
      task_.reset();
    }
  }

  // The stack depth of the next spawned task.
  size_t stack_depth_ = internal::Task::GetMinimalStackSize();

  // The priority of the next spawned task.
  size_t priority_ = 0;

  // Semaphore to track the state of the thread. 0 if running / scheduled, 1 if
  // not.
  fw::os::Semaphore thread_state_sem_;

  // The currently executing task.
  std::optional<internal::Task> task_;
};

}  // namespace fw::os
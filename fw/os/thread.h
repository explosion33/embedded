#pragma once

#include <cstddef>
#include <tuple>
#include <utility>

#include "fw/log/check.h"
#include "fw/os/task_wrapper.h"

namespace fw::os {

// Implements a C++ style thread wrapping FreeRTOS tasks.
// TODO: Add more features: join, cancel, status, etc.
class Thread {
 public:
  Thread() = default;
  Thread(const Thread&) = delete;
  Thread& operator=(const Thread&) = delete;

  // Sets the stack depth for the thread in bytes.
  void SetStackDepth(size_t depth) {
    CHECK(!spawned_);
    stack_depth_ = depth;
  }

  // Sets the priority for the given task, 0 being the lowest.
  void SetPriority(size_t priority) {
    CHECK(!spawned_);
    priority_ = priority;
  }

  // Runs the provided function in the thread with the provided arguments.
  // Can only be called once.
  template <typename Fn, typename... Args>
  void Start(Fn fn, Args... args) {
    CHECK(!spawned_);

    // Convert the args and the function call into a single payload struct.
    using TypedPayload = Payload<Fn, Args...>;
    TypedPayload* payload = new TypedPayload{
        std::move(fn), std::tuple<Args...>(std::move(args)...)};
    CHECK(payload != nullptr);

    const auto entry = [](void* arg) {
      TypedPayload* heap = static_cast<TypedPayload*>(arg);
      TypedPayload local(std::move(*heap));
      delete heap;
      std::apply(local.fn, std::move(local.args));
      internal::ExitCurrentTask();
    };

    internal::CreateTask(internal::TaskConfig{
        .fn = entry,
        .arg = payload,
        .name = "",
        .stack_depth_bytes = stack_depth_,
        .priority = priority_,
    });
    spawned_ = true;
  }

 private:
  // Holds the dynamic typed thread payload.
  template <typename Fn, typename... Args>
  struct Payload {
    Fn fn;
    std::tuple<Args...> args;
  };

  // The stack depth of the next spawned task.
  size_t stack_depth_ = internal::GetMinimalStackSize();

  // The priority of the next spawned task.
  size_t priority_ = 0;

  // True if this thread has been spawned.
  bool spawned_ = false;
};

}  // namespace fw::os
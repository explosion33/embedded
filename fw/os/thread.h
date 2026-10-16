#pragma once

#include <tuple>

#include "FreeRTOS.h"
#include "task.h"

namespace fw::os {

// Implements a C++ style thread wrapping FreeRTOS tasks.
// TODO: Update code to LOG(FATAL) instead of returning false for programming
// errors.
// TODO: Add more features: join, cancel, status, etc.
// TODO: It might be possible to be possible to hide the FreeRTOS dependency
// with a wrapper class: Thread.h -> Task.h -> Task.cc -> FreeRTOS calls.
class Thread {
 public:
  Thread() = default;
  Thread(const Thread&) = delete;
  Thread& operator=(const Thread&) = delete;

  // Sets the stack depth for the thread in bytes.
  bool SetStackDepth(size_t depth) {
    if (handle_ != nullptr) {
      return false;
    }

    if (depth % sizeof(StackType_t) != 0) {
      return false;
    }

    depth /= sizeof(StackType_t);

    if (stack_depth_ == 0) {
      return false;
    }

    stack_depth_ = depth;
    return true;
  }

  // Sets the priority for the given task, 0 being the lowest.
  bool SetPriority(size_t priority) {
    if (handle_ != nullptr) {
      return false;
    }

    if (priority >= configMAX_PRIORITIES) {
      return false;
    }

    priority_ = priority;
    return true;
  }

  // Runs the provided function in the thread with the provided arguments.
  // Can only be called once.
  template <typename Fn, typename... Args>
  bool Start(Fn fn, Args... args) {
    // Cannot start a new task if the current task is in progress.
    if (handle_ != nullptr) {
      // TODO: Replace with LOG(FATAL).
      return false;
    }

    // Convert the args and the function call into a single payload struct.
    auto* payload = new Payload<Fn, Args...>{
        std::move(fn), std::tuple<Args...>(std::move(args)...)};
    if (payload == nullptr) {
      return false;
    }

    // Start the wrapper task, the wrapper task decodes and executes the
    // payload.
    if (xTaskCreate(&Entry<Payload<Fn, Args...>>, "", stack_depth_, payload,
                    priority_, &handle_) != pdPASS) {
      delete payload;
      handle_ = nullptr;
      return false;
    }
    return true;
  }

 private:
  // Holds the dynamic typed thread payload.
  template <typename Fn, typename... Args>
  struct Payload {
    Fn fn;
    std::tuple<Args...> args;
  };

  // FreeRTOS formatted entry task. Unpacks the payload, executes it, and then
  // cleans up the task.
  template <typename P>
  static void Entry(void* param) {
    P* heap = static_cast<P*>(param);
    P local(std::move(*heap));
    delete heap;
    std::apply(local.fn, std::move(local.args));
    vTaskDelete(nullptr);
  }

  // The stack depth of the next spawned task.
  size_t stack_depth_ = configMINIMAL_STACK_SIZE;

  // The priority of the next spawned task.
  size_t priority_ = 0;

  // The handle to the current task.
  TaskHandle_t handle_ = nullptr;
};

}  // namespace fw::os
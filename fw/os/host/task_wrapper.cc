#include "fw/os/task_wrapper.h"

#include <cstdlib>
#include <cstring>
#include <thread>

#include "fw/log/check.h"

namespace fw::os::internal {

namespace {

// Type thrown by ExitCurrentTask and caught by thread wrapup to simulate
// FreeRTOS exit.
struct TaskExit {};
}  // namespace

Task::Task(const Config& task) {
  (void)paused_;
  CHECK(task.fn != nullptr);

  handle_ = new std::thread([fn = task.fn, arg = task.arg] {
    try {
      fn(arg);
    } catch (const TaskExit&) {
      return;
    }
    CHECK(false) << "Task returned without calling ExitCurrentTask()";
  });
}

Task::~Task() {
  // FreeRTOS thread handles can be dropped with no side-effects. This is the
  // equivelent of a detach operation on a std::thread.
  std::thread* thread = static_cast<std::thread*>(handle_);
  thread->detach();
  delete thread;
}

void Task::Pause() { CHECK(false) << "Not supported on host."; }

void Task::Resume() { CHECK(false) << "Not supported on host."; }

void Task::Stop() { CHECK(false) << "Not supported on host."; }

void Task::ExitCurrentTask() { throw TaskExit{}; }

size_t Task::GetMinimalStackSize() {
  // Return arbitrary aligned size. Downstream code will pull this and then use
  // this to determine the stack config, but this implementation does not
  // support dynamically sized stacks.
  return 128;
}

void Task::WaitForNotify() { CHECK(false) << "Not supported on host."; }

void Task::Notify(bool) { CHECK(false) << "Not supported on host."; }

}  // namespace fw::os::internal

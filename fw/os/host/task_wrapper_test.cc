#include "fw/os/task_wrapper.h"

#include "fw/os/time.h"
#include "gtest/gtest.h"

namespace fw::os {
namespace {

TEST(TaskWrapperTest, SpawnAndExitTask) {
  bool ran = false;
  internal::Task task(internal::Task::Config{
      .fn =
          [](void* arg) {
            bool* arg_bool = static_cast<bool*>(arg);
            *arg_bool = true;
            internal::Task::ExitCurrentTask();
          },
      .arg = &ran,
  });

  Delay(100);
  EXPECT_TRUE(ran);
}

}  // namespace
}  // namespace fw::os
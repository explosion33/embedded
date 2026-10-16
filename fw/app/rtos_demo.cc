// Runs two FreeRTOS tasks: one blinks the user LED, the other reports uptime
// and stack usage over the console once a second.

#include <cstdio>
#include <string>
#include <string_view>

#include "fw/log/log.h"
#include "fw/os/main.h"
#include "fw/os/thread.h"
#include "fw/os/time.h"
#include "fw/platform/stm32f4/common/gpio.h"
#include "fw/platform/stm32f4/common/uart.h"
#include "fw/platform/stm32f4/f446/platform.h"

namespace {

constexpr size_t kBlinkPeriodMs = 250;
constexpr size_t kLogPeriodMs = 1000;

// Blinks the UserLED at `kBlinkPeriod`.
void BlinkTask() {
  platform::OutputPin led(platform::kUserLed);
  while (true) {
    led.Toggle();
    fw::os::Delay(kBlinkPeriodMs);
  }
}

// Repeatedly writes hello_world to the console at `kLogPeriod`.
void ConsoleTask(std::string_view base) {
  const std::string base_str(base);
  platform::Uart console(platform::kConsoleUart);

  uint32_t count = 0;
  while (true) {
    LOG_INFO_BLOCKING("%s %d", base_str.c_str(), count);
    count++;
    fw::os::Delay(kLogPeriodMs);
  }
}

}  // namespace

void fw::os::AppMain() {
  {
    fw::os::Thread thread;
    thread.Start(&BlinkTask);
  }

  {
    fw::os::Thread thread;
    thread.SetStackDepth(2048);
    thread.Start(&ConsoleTask, "Hello World");
  }
}

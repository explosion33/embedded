// Runs two FreeRTOS tasks: one blinks the user LED, the other reports uptime
// and stack usage over the console once a second.

#include <cstdio>
#include <string_view>

#include "fw/log/check.h"
#include "fw/os/main.h"
#include "fw/os/thread.h"
#include "fw/os/time.h"
#include "fw/platform/stm32f4/common/gpio.h"
#include "fw/platform/stm32f4/common/uart.h"
#include "fw/platform/stm32f4/f446/platform.h"

// TODO: Replace the raw FreeRTOS API with the OS abstraction layer once it
// exists.
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
  platform::Uart console(platform::kConsoleUart);

  constexpr size_t kMaxDigits = 3;
  constexpr size_t kMaxValue = 1000;

  // Base + " " + digits + "\r\n";
  const size_t size = base.size() + kMaxDigits + 3;
  char* text = new char[size];

  uint32_t count = 0;
  while (true) {
    snprintf(text, size, "%s %d\r\n", base.cbegin(), count);
    console.Write(text);
    count++;
    count %= kMaxValue;
    fw::os::Delay(kLogPeriodMs);
  }
}

}  // namespace

void fw::os::AppMain() {
  {
    fw::os::Thread thread;
    CHECK(thread.Start(&BlinkTask)) << "Failed to start blink thread.";
  }

  {
    fw::os::Thread thread;
    thread.SetStackDepth(2048);
    CHECK(thread.Start(&ConsoleTask, "Hello World"))
        << "Failed to start console thread.";
  }
}

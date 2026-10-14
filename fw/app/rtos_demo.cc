// Runs two FreeRTOS tasks: one blinks the user LED, the other reports uptime
// and stack usage over the console once a second.

#include <atomic>
#include <cstdint>
#include <cstdio>

#include "FreeRTOS.h"
#include "task.h"

#include "fw/platform/stm32f4/common/gpio.h"
#include "fw/platform/stm32f4/common/uart.h"
#include "fw/platform/stm32f4/f446/platform.h"

// TODO: Replace the raw FreeRTOS API with the OS abstraction layer once it
// exists.
namespace {

constexpr size_t kBlinkPeriod = 250;
constexpr size_t kLogPeriod = 1000;

// Stack depths are in words. The console task's is larger because snprintf
// uses a lot of stack.
constexpr configSTACK_DEPTH_TYPE kBlinkStackDepth = configMINIMAL_STACK_SIZE;
constexpr configSTACK_DEPTH_TYPE kConsoleStackDepth = 512;

// Blinks the UserLED at `kBlinkPeriod`.
void BlinkTask(void* arg) {
  platform::OutputPin led(platform::kUserLed);
  while (true) {
    led.Toggle();
    vTaskDelay(kBlinkPeriod);
  }
}

// The only task that formats text or touches the console, since newlib is not
// configured to be reentrant.

// Repeatedly writes hello_world to the console at `kLogPeriod`.
void ConsoleTask(void* arg) {
  platform::Uart console(platform::kConsoleUart);

  // Base message + trailing incrementing digits + \r\n.
  constexpr char kBaseStr[] = "Hello World ";
  constexpr size_t kNumDigits = 3;
  constexpr size_t kTotalBytes = sizeof(kBaseStr) + kNumDigits + 2;

  static_assert(kNumDigits > 0);
  size_t max_count = 9;
  for (size_t i = 0; i < kNumDigits - 1; i++) {
    max_count *= 10;
    max_count += 9;
  }

  char text[kTotalBytes];
  size_t count = 0;
  while (true) {
    snprintf(text, sizeof(text), "%s%d\r\n", kBaseStr, count);
    console.Write(text);

    count++;
    count %= max_count;

    vTaskDelay(kLogPeriod);
  }
}

}  // namespace

int main() {
  configASSERT(xTaskCreate(BlinkTask, "blink", kBlinkStackDepth, nullptr, 0,
                           nullptr) == pdPASS);

  configASSERT(xTaskCreate(ConsoleTask, "console", kConsoleStackDepth, nullptr,
                           0, nullptr) == pdPASS);

  vTaskStartScheduler();
  configASSERT(false);
}

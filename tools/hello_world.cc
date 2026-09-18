// TODO: Simple Hello World app to validate STM builds. Move to app/.

#include "platform/stm32f4/common/uart.h"
#include "platform/stm32f4/f446/platform.h"

int main() {
  platform::Uart console(platform::kConsoleUart);

  while (true) {
    console.Write("hello world\r\n");
    platform::BusyWait(1'000'000);
  }
}

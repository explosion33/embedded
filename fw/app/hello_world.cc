#include <cstdio>

#include "fw/platform/stm32f4/common/uart.h"
#include "fw/platform/stm32f4/f446/platform.h"

int main() {
  platform::Uart console(platform::kConsoleUart);

  char text[20];
  uint8_t val = 0;
  while (true) {
    sprintf(text, "Hello World %u\r\n", val);
    val++;
    val = val % 100;

    console.Write(text);
    platform::BusyWait(1'000'000);
  }
}

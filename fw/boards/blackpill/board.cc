#include "fw/boards/blackpill/board.h"

#include "fw/platform/stm32f4/common/pin_map.h"
#include "fw/platform/stm32f4/common/uart.h"

namespace fw::board {

namespace {
namespace pins = fw::platform::pins;

// The blackpill does not expose a dedicated UART Console output. This assumes
// the blackpill has PA2 / PA3 wired to a some USB Serial device. UART
// configuration for USART2 (PA2/PA3).
constexpr platform::Uart::Config kConsoleUart = {
    .uart = pins::Uart::kUSART2,
    .tx = pins::Pin::kPA2,
    .rx = pins::Pin::kPA3,
    .baud_rate = 115200,
};

}  // namespace

platform::Uart& Console() {
  static platform::Uart console(kConsoleUart);
  return console;
}

}  // namespace fw::board

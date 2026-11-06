#include "fw/boards/nucleo_f446re/board.h"

#include "fw/platform/stm32f4/common/pin_map.h"
#include "fw/platform/stm32f4/common/uart.h"

namespace fw::board {

namespace {
namespace pins = fw::platform::pins;

// UART configuration for USART2 (PA2/PA3), which is wired to the STLINK virtual
// COM port.
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

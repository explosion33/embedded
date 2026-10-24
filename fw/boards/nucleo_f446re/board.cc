#include "fw/boards/nucleo_f446re/board.h"

#include "fw/platform/stm32f4/common/uart.h"
#include "stm32f4xx_ll_bus.h"
#include "stm32f4xx_ll_gpio.h"

namespace fw::board {

namespace {
// UART configuration for USART2 (PA2/PA3), which is wired to the STLINK virtual
// COM port.
constexpr platform::UartConfig kConsoleUart = {
    .usart = USART2,
    .usart_clock = LL_APB1_GRP1_PERIPH_USART2,
    .port = GPIOA,
    .port_clock = LL_AHB1_GRP1_PERIPH_GPIOA,
    .tx_pin = LL_GPIO_PIN_2,
    .rx_pin = LL_GPIO_PIN_3,
    .alternate = LL_GPIO_AF_7,
    .baud_rate = 115200,
};

}  // namespace

platform::Uart& Console() {
  static platform::Uart console(kConsoleUart);
  return console;
}

}  // namespace fw::board

#pragma once

// Board and part configuration for the STM32F446RE.

#include <cstdint>

#include "platform/stm32f4/common/uart.h"
#include "stm32f4xx_ll_bus.h"
#include "stm32f4xx_ll_gpio.h"

namespace platform {

// By default the MCU boots on the 16MHz HSI, with no PLLs configured.
constexpr uint32_t kDefaultSysClkHz = 16'000'000;

// UART configuration for USART2 (PA2/PA3). On a Nucleo-F446RE these pins are
// wired to the STLINK virtual COM port.
// TODO: This definition should not live here. This is STM32F446 generic code,
// but assumes device specifc. Additional per-device targets should be created
// and maintained.
constexpr UartConfig kConsoleUart = {
    .usart = USART2,
    .usart_clock = LL_APB1_GRP1_PERIPH_USART2,
    .port = GPIOA,
    .port_clock = LL_AHB1_GRP1_PERIPH_GPIOA,
    .tx_pin = LL_GPIO_PIN_2,
    .rx_pin = LL_GPIO_PIN_3,
    .alternate = LL_GPIO_AF_7,
    .baud_rate = 115200,
};

// Waits roughly `iterations` cycles.
// TODO: Replace with FreeRtos based sleeps.
inline void BusyWait(uint32_t iterations) {
  for (uint32_t i = 0; i < iterations; ++i) {
    __asm__ volatile("nop");
  }
}

}  // namespace platform

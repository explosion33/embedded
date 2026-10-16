#pragma once

// Board configuration for the Nucleo-F446RE.

#include "fw/platform/stm32f4/common/gpio.h"
#include "fw/platform/stm32f4/common/uart.h"
#include "stm32f4xx_ll_bus.h"
#include "stm32f4xx_ll_gpio.h"

namespace fw::board {

// The user LED, LD2 (PA5).
constexpr platform::OutputPinConfig kUserLed = {
    .port = GPIOA,
    .port_clock = LL_AHB1_GRP1_PERIPH_GPIOA,
    .pin = LL_GPIO_PIN_5,
};

// Returns the console UART, configured from `kConsoleUart` on first use. This
// is the only instance of the console, shared by the log backend and apps.
platform::Uart& Console();

}  // namespace fw::board

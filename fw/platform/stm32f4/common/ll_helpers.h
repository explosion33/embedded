#pragma once

#include "fw/log/check.h"
#include "fw/platform/stm32f4/common/pin_map.h"
#include "stm32f4xx_ll_bus.h"
#include "stm32f4xx_ll_gpio.h"

namespace fw::platform::internal {

// Get the LL Port def pointer for this port.
inline GPIO_TypeDef* GetLLPort(pins::Port port) {
  constexpr uintptr_t kStride = GPIOB_BASE - GPIOA_BASE;
  return reinterpret_cast<GPIO_TypeDef*>(
      GPIOA_BASE + static_cast<uintptr_t>(port) * kStride);
}

// Gets the LL port clock mask for the provided port.
constexpr uint32_t GetLLPortClock(pins::Port port) {
  return LL_AHB1_GRP1_PERIPH_GPIOA << static_cast<size_t>(port);
}

// Gets the LL pin mask for the provided pin.
constexpr uint32_t GetLLPin(uint8_t pin_idx) {
  return LL_GPIO_PIN_0 << pin_idx;
}

// Gets the LL USART type definition from the provided uart.
constexpr USART_TypeDef* GetLLUsart(pins::Uart uart) {
  switch (static_cast<uint8_t>(uart)) {
    case 1:
      return USART1;
    case 2:
      return USART2;
#if defined(USART3)
    case 3:
      return USART3;
#endif
#if defined(UART4)
    case 4:
      return UART4;
#endif
#if defined(UART5)
    case 5:
      return UART5;
#endif
    case 6:
      return USART6;
    default:
      CHECK(false);
  }
}

// Gets the LL port clock mask for the provided port.
constexpr uint32_t GetLLUsartClock(pins::Uart uart) {
  switch (static_cast<uint8_t>(uart)) {
    case 1:
      return LL_APB2_GRP1_PERIPH_USART1;
    case 2:
      return LL_APB1_GRP1_PERIPH_USART2;
#if defined(USART3)
    case 3:
      return LL_APB1_GRP1_PERIPH_USART3;
#endif
#if defined(UART4)
    case 4:
      return LL_APB1_GRP1_PERIPH_UART4;
#endif
#if defined(UART5)
    case 5:
      return LL_APB1_GRP1_PERIPH_UART5;
#endif
    case 6:
      return LL_APB2_GRP1_PERIPH_USART6;
    default:
      CHECK(false);
  }
}

}  // namespace fw::platform::internal
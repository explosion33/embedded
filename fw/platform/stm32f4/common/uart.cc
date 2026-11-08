#include "fw/platform/stm32f4/common/uart.h"

#include <cstddef>
#include <cstdint>

#include "fw/log/check.h"
#include "fw/platform/stm32f4/common/ll_helpers.h"
#include "stm32f4xx_ll_bus.h"
#include "stm32f4xx_ll_gpio.h"
#include "stm32f4xx_ll_usart.h"

namespace fw::platform {

Uart::Uart(const Config& config) : lock_({config.tx, config.rx}) {
  USART_TypeDef* usart = internal::GetLLUsart(config.uart);
  handle_ = usart;

  const pins::Port port = pins::GetPort(config.tx);
  CHECK(pins::GetPort(config.rx) == port);

  const auto maybe_af_rx = pins::GetUartRxAf(config.uart, config.rx);
  const auto maybe_af_tx = pins::GetUartTxAf(config.uart, config.tx);
  CHECK(maybe_af_rx.has_value());
  CHECK(maybe_af_tx.has_value());
  CHECK(maybe_af_rx.value() == maybe_af_tx.value());

  LL_AHB1_GRP1_EnableClock(internal::GetLLPortClock(port));
  LL_APB1_GRP1_EnableClock(internal::GetLLUsartClock(config.uart));

  LL_GPIO_InitTypeDef gpio_init = {
      .Pin = internal::GetLLPin(pins::GetPinIndex(config.rx)) |
             internal::GetLLPin(pins::GetPinIndex(config.tx)),
      .Mode = LL_GPIO_MODE_ALTERNATE,
      .Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH,
      .OutputType = LL_GPIO_OUTPUT_PUSHPULL,
      .Pull = LL_GPIO_PULL_NO,
      .Alternate = maybe_af_rx.value(),
  };
  LL_GPIO_Init(internal::GetLLPort(port), &gpio_init);

  LL_USART_InitTypeDef usart_init = {
      .BaudRate = config.baud_rate,
      .DataWidth = LL_USART_DATAWIDTH_8B,
      .StopBits = LL_USART_STOPBITS_1,
      .Parity = LL_USART_PARITY_NONE,
      .TransferDirection = LL_USART_DIRECTION_TX_RX,
      .HardwareFlowControl = LL_USART_HWCONTROL_NONE,
      .OverSampling = LL_USART_OVERSAMPLING_16,
  };
  LL_USART_Init(usart, &usart_init);
  LL_USART_ConfigAsyncMode(usart);
  LL_USART_Enable(usart);
}

void Uart::Write(const uint8_t* data, size_t size) {
  for (size_t i = 0; i < size; ++i) {
    // TXE goes high once the data register has been copied into the shift
    // register, so this is the point at which the next byte may be written.
    while (!LL_USART_IsActiveFlag_TXE(static_cast<USART_TypeDef*>(handle_))) {
    }
    LL_USART_TransmitData8(static_cast<USART_TypeDef*>(handle_), data[i]);
  }

  // Wait for the final byte to leave the shift register. Without this a caller
  // that resets or sleeps immediately after Write would truncate the output.
  while (!LL_USART_IsActiveFlag_TC(static_cast<USART_TypeDef*>(handle_))) {
  }
}

void Uart::Write(const char* text) {
  size_t size = 0;
  while (text[size] != '\0') {
    ++size;
  }
  Write(reinterpret_cast<const uint8_t*>(text), size);
}

}  // namespace fw::platform

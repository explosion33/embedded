#include "fw/platform/stm32f4/common/uart.h"

#include <cstddef>
#include <cstdint>

#include "stm32f4xx_ll_bus.h"
#include "stm32f4xx_ll_gpio.h"
#include "stm32f4xx_ll_usart.h"

namespace platform {

Uart::Uart(const UartConfig& config) : usart_(config.usart) {
  LL_AHB1_GRP1_EnableClock(config.port_clock);
  LL_APB1_GRP1_EnableClock(config.usart_clock);

  LL_GPIO_InitTypeDef gpio_init = {
      .Pin = config.tx_pin | config.rx_pin,
      .Mode = LL_GPIO_MODE_ALTERNATE,
      .Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH,
      .OutputType = LL_GPIO_OUTPUT_PUSHPULL,
      .Pull = LL_GPIO_PULL_UP,
      .Alternate = config.alternate,
  };
  LL_GPIO_Init(config.port, &gpio_init);

  LL_USART_InitTypeDef usart_init = {
      .BaudRate = config.baud_rate,
      .DataWidth = LL_USART_DATAWIDTH_8B,
      .StopBits = LL_USART_STOPBITS_1,
      .Parity = LL_USART_PARITY_NONE,
      .TransferDirection = LL_USART_DIRECTION_TX_RX,
      .HardwareFlowControl = LL_USART_HWCONTROL_NONE,
      .OverSampling = LL_USART_OVERSAMPLING_16,
  };
  LL_USART_Init(usart_, &usart_init);

  // Clears the synchronous, smartcard, half-duplex and IrDA bits that
  // LL_USART_Init leaves alone.
  LL_USART_ConfigAsyncMode(usart_);

  LL_USART_Enable(usart_);
}

void Uart::Write(const uint8_t* data, size_t size) {
  for (size_t i = 0; i < size; ++i) {
    // TXE goes high once the data register has been copied into the shift
    // register, so this is the point at which the next byte may be written.
    while (!LL_USART_IsActiveFlag_TXE(usart_)) {
    }
    LL_USART_TransmitData8(usart_, data[i]);
  }

  // Wait for the final byte to leave the shift register. Without this a caller
  // that resets or sleeps immediately after Write would truncate the output.
  while (!LL_USART_IsActiveFlag_TC(usart_)) {
  }
}

void Uart::Write(const char* text) {
  size_t size = 0;
  while (text[size] != '\0') {
    ++size;
  }
  Write(reinterpret_cast<const uint8_t*>(text), size);
}

}  // namespace platform

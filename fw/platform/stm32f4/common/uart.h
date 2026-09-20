#pragma once

#include <cstddef>
#include <cstdint>

#include "stm32f4xx_ll_gpio.h"
#include "stm32f4xx_ll_usart.h"

// TODO: This file is very clauded. This should be cleaned up and moved to a
// standard interface to be shared across platforms.
namespace platform {

// Struct containing the config required to bringup uart.
struct UartConfig {
  // The uart peripheral to configure.
  USART_TypeDef* usart;

  // The bit to set in the RCC APB register to clock uart.
  // TODO: Add better clock definition / comment.
  uint32_t usart_clock;

  // The GPIO port for the uart pins.
  GPIO_TypeDef* port;

  // the bit to set in the RCC APB register to clock the GPIO port.
  // TODO: Add better clock definition / comment.
  uint32_t port_clock;

  // Transmit and receive pins on `port`, e.g. LL_GPIO_PIN_2, and the
  // alternate function that maps them to `usart`, e.g. LL_GPIO_AF_7.

  // GPIO pin on `port` to host TX on.
  uint32_t tx_pin;

  // GPIO pin on `port` to host RX on.
  uint32_t rx_pin;

  // TODO: what is this.
  uint32_t alternate;

  // The baud rate to host UART at.
  uint32_t baud_rate;
};

// Creates a uart for an stm32f4 MCU.
class Uart {
 public:
  // Configures Uart with the provided config.
  explicit Uart(const UartConfig& config);

  // Blocks until every byte has been accepted by the transmitter.

  // Writes the buffer to the UART transmitter, blocking until the write has
  // completed.
  void Write(const uint8_t* data, size_t size);

  // Writes a null terminated string, without the terminator to the UART
  // transmitter.
  void Write(const char* text);

 private:
  USART_TypeDef* usart_;
};

}  // namespace platform

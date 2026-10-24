#pragma once

#include <cstdint>

#include "fw/platform/peripherals/gpio_output_interface.h"
#include "stm32f4xx_ll_gpio.h"

namespace fw::platform {

// The config required to drive a GPIO pin as a push-pull output.
struct OutputPinConfig {
  // The GPIO port the pin is on.
  GPIO_TypeDef* port;

  // The bit to set in the RCC AHB1 register to clock `port`, e.g.
  // LL_AHB1_GRP1_PERIPH_GPIOA.
  uint32_t port_clock;

  // The pin on `port`, e.g. LL_GPIO_PIN_5.
  uint32_t pin;
};

// A GPIO pin configured as a push-pull output, initially low.
class GpioOutput : public GpioOutputInterface {
 public:
  struct Config {};

  explicit GpioOutput(const OutputPinConfig& config);

  // Drives the pin high when `high`, otherwise low.
  void Set(bool high);

  // Inverts the pin's current output level.
  void Toggle();

 private:
  // The GPIO port this GpioOutput uses.
  GPIO_TypeDef* port_;

  // The pin this GpioOutput uses.
  uint32_t pin_;
};

}  // namespace fw::platform

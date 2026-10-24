#pragma once

#include "fw/platform/peripherals/gpio_output_interface.h"
#include "fw/platform/peripherals/pin_map.h"

namespace fw::platform {

// A GPIO pin configured as a push-pull output, initially low.
class GpioOutput : public GpioOutputInterface {
 public:
  explicit GpioOutput(Pin pin);

  // Drives the pin high when `high`, otherwise low.
  void Set(bool high);

  // Inverts the pin's current output level.
  void Toggle();

 private:
  // The GPIO port for this output pin.
  const Port port_;

  // The pin for this output pin.
  const uint8_t pin_;
};

}  // namespace fw::platform

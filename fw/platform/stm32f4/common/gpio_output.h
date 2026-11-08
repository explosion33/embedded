#pragma once

#include "fw/platform/common/gpio_output_interface.h"
#include "fw/platform/common/pin_lock.h"
#include "fw/platform/stm32f4/common/pin_map.h"

namespace fw::platform {

// A GPIO pin configured as a push-pull output, initially low.
class GpioOutput : public GpioOutputInterface {
 public:
  explicit GpioOutput(pins::Pin pin);

  // Drives the pin high when `high`, otherwise low.
  void Set(bool high);

  // Inverts the pin's current output level.
  void Toggle();

 private:
  // The GPIO port for this output pin.
  const pins::Port port_;

  // The pin for this output pin.
  const uint8_t pin_;

  // The pin lock for this GPIO pin
  PinLock<pins::Pin>::Lock lock_;
};

}  // namespace fw::platform

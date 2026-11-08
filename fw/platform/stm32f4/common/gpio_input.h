#pragma once

#include "fw/platform/common/gpio_input_interface.h"
#include "fw/platform/common/pin_lock.h"
#include "fw/platform/stm32f4/common/pin_map.h"

namespace fw::platform {

// A Digital Input GPIO Pin.
class GpioInput : public GpioInputInterface {
 public:
  explicit GpioInput(pins::Pin pin);

  bool Read() const override;
  void SetPull(Pull pull) override;

 private:
  // The GPIO port for this input pin.
  const pins::Port port_;

  // The pin for this input pin.
  const uint8_t pin_;

  // The pin lock for this GPIO pin
  PinLock<pins::Pin>::Lock lock_;
};

}  // namespace fw::platform

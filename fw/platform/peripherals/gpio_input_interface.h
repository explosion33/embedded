#pragma once

namespace fw::platform {

// Defines a GPIO digital ouptut for a given pin.
class GpioInputInterface {
 public:
  // Reads the state of the pin.
  virtual bool Read() const = 0;
};
}  // namespace fw::platform
#pragma once

namespace fw::platform {

// Defines a GPIO digital ouptut for a given pin.
class GpioOutputInterface {
 public:
  // Sets the state of the pin.
  virtual void Set(bool high) = 0;

  // Toggles the state of the pin.
  virtual void Toggle() = 0;
};
}  // namespace fw::platform
#pragma once

#include <cstdint>
namespace fw::platform {

// Defines a GPIO digital ouptut for a given pin.
class GpioInputInterface {
 public:
  enum class Pull : uint8_t {
    kNone,
    kUp,
    kDown,
  };

  // Reads the state of the pin.
  virtual bool Read() const = 0;

  // Sets the Pull state of the pin.
  virtual void SetPull(Pull pull) = 0;
};
}  // namespace fw::platform
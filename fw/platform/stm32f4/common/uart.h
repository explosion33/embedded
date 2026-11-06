#pragma once

#include <cstddef>
#include <cstdint>

#include "fw/platform/stm32f4/common/pin_map.h"

namespace fw::platform {

// Creates a uart for an stm32f4 MCU.
// TODO: Expose more uart construction and runtime configuration as needed.
class Uart {
 public:
  // Configuration for one uart peripheral.
  struct Config {
    pins::Uart uart;
    pins::Pin tx;
    pins::Pin rx;
    uint32_t baud_rate;
  };

  // Configures Uart with the provided config.
  explicit Uart(const Config& config);

  // Writes the buffer to the UART transmitter, blocking until the write has
  // completed.
  void Write(const uint8_t* data, size_t size);

  // Writes a null terminated string, without the terminator to the UART
  // transmitter.
  void Write(const char* text);

 private:
  // Type erased usart handle.
  void* handle_;
};

}  // namespace fw::platform

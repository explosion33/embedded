#include "fw/os/time.h"

#include <chrono>
#include <thread>

namespace fw::os {

void Delay(size_t millis) {
  std::this_thread::sleep_for(std::chrono::milliseconds(millis));
}

size_t Millis() {
  static const auto start_time = std::chrono::steady_clock::now();
  const auto curr_time = std::chrono::steady_clock::now() - start_time;
  return std::chrono::duration_cast<std::chrono::milliseconds>(curr_time)
      .count();
}

}  // namespace fw::os
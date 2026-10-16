#pragma once
#include <cstddef>

namespace fw::os {

// Delays the current thread for `millis` milliseconds.
void Delay(size_t millis);

}  // namespace fw::os
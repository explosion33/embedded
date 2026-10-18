#pragma once
#include <cstddef>

namespace fw::os {

// Delays the current thread for `millis` milliseconds.
void Delay(size_t millis);

// Gets the current time in milliseconds.
size_t Millis();

}  // namespace fw::os
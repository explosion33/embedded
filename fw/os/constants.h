#pragma once

#include <cstddef>

namespace fw::os {

// Gets the maximum priority of an os system call.
size_t GetMaxSysCallInterruptPriority();
}  // namespace fw::os
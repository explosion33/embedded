#include "fw/log/log.h"

#include <algorithm>
#include <cstdarg>
#include <cstddef>
#include <cstdio>
#include <string_view>

#include "fw/log/backend.h"

namespace fw::log::internal {

namespace {
constexpr size_t kMaxMessageSize = 256;
}

[[noreturn]] void Fatal(const char* file, int line, const char* fmt, ...) {
  // If the backend itself fails while reporting, skip straight to halting
  // rather than recursing.
  static bool in_fatal = false;
  if (in_fatal) {
    backend::Halt();
  }
  in_fatal = true;

  // Static rather than on the stack, as the stack may be what has failed.
  static char message[kMaxMessageSize];
  size_t size = 0;

  int written = snprintf(message, sizeof(message), "FATAL %s:%d: ", file, line);
  if (written > 0) {
    size = std::min(static_cast<size_t>(written), sizeof(message) - 1);
  }

  va_list args;
  va_start(args, fmt);
  written = vsnprintf(message + size, sizeof(message) - size, fmt, args);
  va_end(args);
  if (written > 0) {
    size = std::min(size + static_cast<size_t>(written), sizeof(message) - 1);
  }

  backend::Write(std::string_view(message, size));
  backend::Write("\r\n");
  backend::Halt();
}

}  // namespace fw::log::internal

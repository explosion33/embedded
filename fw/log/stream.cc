#include "fw/log/stream.h"

#include <string_view>

namespace fw::log::internal {

std::string_view FormatNumber(unsigned long long value, unsigned base,
                              char (&buf)[kMaxNumberDigits]) {
  constexpr std::string_view kDigits = "0123456789abcdef";
  char* const end = buf + kMaxNumberDigits;
  char* start = end;
  do {
    *--start = kDigits[value % base];
    value /= base;
  } while (value != 0);
  return std::string_view(start, end - start);
}

}  // namespace fw::log::internal

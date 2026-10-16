#include "fw/log/check.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <string_view>

#include "fw/log/log.h"

namespace fw::check::internal {

char FatalStream::message_[FatalStream::kMaxMessageSize];
size_t FatalStream::message_size_ = 0;

FatalStream::FatalStream(const char* file, int line, const char* condition)
    : file_(file), line_(line) {
  message_size_ = 0;
  Append("Check failed: ");
  Append(condition);
  Append(" ");
}

FatalStream::~FatalStream() {
  message_[message_size_] = '\0';
  ::fw::log::internal::Fatal(file_, line_, "%s", message_);
}

FatalStream& FatalStream::operator<<(const char* s) {
  Append(s != nullptr ? s : "(null)");
  return *this;
}

FatalStream& FatalStream::operator<<(std::string_view s) {
  Append(s);
  return *this;
}

FatalStream& FatalStream::operator<<(bool b) {
  Append(b ? "true" : "false");
  return *this;
}

FatalStream& FatalStream::operator<<(char c) {
  Append(std::string_view(&c, 1));
  return *this;
}

FatalStream& FatalStream::operator<<(const void* p) {
  Append("0x");
  AppendNumber(reinterpret_cast<uintptr_t>(p), 16);
  return *this;
}

void FatalStream::Append(std::string_view text) {
  // Leave room for the terminator added before handing off to LOG_FATAL.
  const size_t size =
      std::min(text.size(), sizeof(message_) - 1 - message_size_);
  memcpy(message_ + message_size_, text.data(), size);
  message_size_ += size;
}

// Formats by hand rather than with snprintf, as long long support is often
// missing from embedded libc builds.
void FatalStream::AppendNumber(unsigned long long value, unsigned base) {
  constexpr std::string_view kDigits = "0123456789abcdef";
  // Enough for a 64-bit value in base 10.
  char digits[20];
  char* const end = digits + sizeof(digits);
  char* start = end;
  do {
    *--start = kDigits[value % base];
    value /= base;
  } while (value != 0);
  Append(std::string_view(start, end - start));
}

void FatalStream::AppendSigned(long long value) {
  if (value < 0) {
    Append("-");
    // Negate as unsigned, so the minimum value does not overflow.
    AppendNumber(0ULL - static_cast<unsigned long long>(value), 10);
  } else {
    AppendNumber(value, 10);
  }
}

void FatalStream::AppendUnsigned(unsigned long long value) {
  AppendNumber(value, 10);
}

}  // namespace fw::check::internal

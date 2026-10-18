#pragma once

#include <concepts>
#include <cstddef>
#include <cstdint>
#include <string_view>
#include <type_traits>

namespace fw::log {

namespace internal {

// Enough digits for a 64-bit value in any base from 10 to 16.
inline constexpr size_t kMaxNumberDigits = 20;

// Formats `value` in `base` (10 to 16) into the tail of `buf`, returning the
// written digits.
std::string_view FormatNumber(unsigned long long value, unsigned base,
                              char (&buf)[kMaxNumberDigits]);

}  // namespace internal

// Adds `<<` streaming to `Derived`, which must provide
// `Append(std::string_view)`. Values are formatted to text and passed to
// Append in order.
template <typename Derived>
class Stream {
 public:
  Derived& operator<<(std::string_view s) { return Put(s); }

  Derived& operator<<(const char* s) {
    return Put(s != nullptr ? s : "(null)");
  }

  Derived& operator<<(bool b) { return Put(b ? "true" : "false"); }

  Derived& operator<<(char c) { return Put(std::string_view(&c, 1)); }

  Derived& operator<<(const void* p) {
    char buf[internal::kMaxNumberDigits];
    Put("0x");
    return Put(internal::FormatNumber(reinterpret_cast<uintptr_t>(p), 16, buf));
  }

  template <std::integral T>
  Derived& operator<<(T v) {
    char buf[internal::kMaxNumberDigits];
    auto magnitude = static_cast<unsigned long long>(v);
    if constexpr (std::is_signed_v<T>) {
      if (v < 0) {
        Put("-");
        // Negate as unsigned, so the minimum value does not overflow.
        magnitude = 0ULL - magnitude;
      }
    }
    return Put(internal::FormatNumber(magnitude, 10, buf));
  }

 private:
  // Hands `text` to the derived class.
  Derived& Put(std::string_view text) {
    Derived& self = static_cast<Derived&>(*this);
    self.Append(text);
    return self;
  }
};

}  // namespace fw::log

#pragma once

#include <concepts>
#include <cstddef>
#include <string_view>
#include <type_traits>

// Fatal Errors when `cond` is false. Supports streaming error messages with
// `<<`.
#define CHECK(cond)                           \
  (cond) ? (void)0                            \
         : ::fw::check::internal::Voidify() & \
               ::fw::check::internal::FatalStream(__FILE__, __LINE__, #cond)

namespace fw::check::internal {

// Accumulates the message for a failed CHECK. The destructor reports it through
// LOG_FATAL and halts.
class FatalStream {
 public:
  // Writes "Check failed: cond " into the static buffer.
  FatalStream(const char* file, int line, const char* condition);
  // Writes the buffer and halts.
  [[noreturn]] ~FatalStream();

  FatalStream(const FatalStream&) = delete;
  FatalStream& operator=(const FatalStream&) = delete;

  FatalStream& operator<<(const char* s);
  FatalStream& operator<<(std::string_view s);
  FatalStream& operator<<(bool b);
  FatalStream& operator<<(char c);
  FatalStream& operator<<(const void* p);

  template <std::integral T>
  FatalStream& operator<<(T v) {
    if constexpr (std::is_signed_v<T>) {
      AppendSigned(v);
    } else {
      AppendUnsigned(v);
    }
    return *this;
  }

 private:
  static constexpr size_t kMaxMessageSize = 256;

  // Appends `text` to the message.
  static void Append(std::string_view text);

  // Appends a number to the message with the provided base.
  static void AppendNumber(unsigned long long value, unsigned base);

  // Appends a signed number to the message.
  static void AppendSigned(long long value);

  // Appends an usigned number to the message.
  static void AppendUnsigned(unsigned long long value);

  // The message this FatalStream encodes, static since the stack may have
  // failed.
  static char message_[kMaxMessageSize];

  // The current size of the stored message.
  static size_t message_size_;

  // The file this fatal error occured at.
  const char* file_;

  // The line this fatal error occured at.
  int line_;
};

// operator& binds more loosely than <<, so the whole << chain is evaluated
// first, and both sides of the ?: end up as void.
struct Voidify {
  void operator&(const FatalStream&) {}
};

}  // namespace fw::check::internal

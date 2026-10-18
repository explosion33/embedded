#pragma once

#include <cstddef>
#include <string_view>

#include "fw/log/stream.h"

// Fatal Errors when `cond` is false. Supports streaming error messages with
// `<<`.
#define CHECK(cond)                           \
  (cond) ? (void)0                            \
         : ::fw::check::internal::Voidify() & \
               ::fw::check::internal::FatalStream(__FILE__, __LINE__, #cond)

namespace fw::check::internal {

// Accumulates the message for a failed CHECK. The destructor reports it through
// LOG_FATAL and halts.
class FatalStream : public ::fw::log::Stream<FatalStream> {
 public:
  // Writes "Check failed: cond " into the static buffer.
  FatalStream(const char* file, int line, const char* condition);
  // Writes the buffer and halts.
  [[noreturn]] ~FatalStream();

  FatalStream(const FatalStream&) = delete;
  FatalStream& operator=(const FatalStream&) = delete;

 private:
  friend class ::fw::log::Stream<FatalStream>;

  static constexpr size_t kMaxMessageSize = 256;

  // Appends `text` to the message.
  static void Append(std::string_view text);

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

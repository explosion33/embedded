#include "fw/log/check.h"

#include <algorithm>
#include <cstddef>
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

void FatalStream::Append(std::string_view text) {
  // Leave room for the terminator added before handing off to LOG_FATAL.
  const size_t size =
      std::min(text.size(), sizeof(message_) - 1 - message_size_);
  memcpy(message_ + message_size_, text.data(), size);
  message_size_ += size;
}

}  // namespace fw::check::internal

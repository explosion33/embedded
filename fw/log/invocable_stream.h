#pragma once
#include <functional>
#include <string>
#include <string_view>

#include "fw/log/stream.h"

namespace fw::log {
// Streaming text class that captures the streamed data and returns it via
// callback.
class InvolableStream : public ::fw::log::Stream<InvolableStream> {
 public:
  explicit InvolableStream(std::function<void(std::string_view)> fn)
      : fn_(fn) {};
  ~InvolableStream();

  InvolableStream(const InvolableStream&) = delete;
  InvolableStream& operator=(const InvolableStream&) = delete;

 private:
  // The stream interface needs access to Append() which is private.
  friend class ::fw::log::Stream<InvolableStream>;

  // Appends the provided msg to the backing `text_`.
  void Append(std::string_view msg);

  // The callback to invoke on destruction.
  const std::function<void(std::string_view)> fn_;

  // The current captured stream.
  std::string text_;
};
}  // namespace fw::log
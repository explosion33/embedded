#include "fw/log/invocable_stream.h"

namespace fw::log {

InvolableStream::~InvolableStream() { fn_(text_); }
void InvolableStream::Append(std::string_view msg) { text_.append(msg); }

}  // namespace fw::log
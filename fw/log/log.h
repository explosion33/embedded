#pragma once

#define LOG_FATAL(fmt, ...) \
  ::fw::log::internal::Fatal(__FILE__, __LINE__, fmt __VA_OPT__(, ) __VA_ARGS__)

namespace fw::log::internal {
[[noreturn]] void Fatal(const char* file, int line, const char* fmt, ...)
    __attribute__((format(printf, 3, 4)));

}
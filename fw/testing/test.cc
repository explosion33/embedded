#include "fw/testing/test.h"

#include <string_view>

#include "fw/log/log.h"

namespace fw::testing {
namespace {

constexpr std::string_view kTestPrefix = "[FW_TEST]";
constexpr std::string_view kTestSuffix = "[END_TEST]";
constexpr std::string_view kTestPass = "<PASS>";
constexpr std::string_view kTestFail = "<FAIL>";

enum class LogType {
  kMessage,
  kPass,
  kFail,
};

#define STRING_LOG(str) \
  LOG_BLOCKING("%.*s", static_cast<int>(str.length()), str.data());

void LogImpl(std::string_view msg, LogType type) {
  STRING_LOG(kTestPrefix);
  switch (type) {
    case LogType::kMessage:
      break;
    case LogType::kPass:
      STRING_LOG(kTestPass);
      break;
    case LogType::kFail:
      STRING_LOG(kTestFail);
      break;
  }
  STRING_LOG(msg);
  STRING_LOG(kTestSuffix);
  LOG_BLOCKING("\r\n");
}

}  // namespace

void LogTest(std::string_view msg) { LogImpl(msg, LogType::kMessage); }

void TestPass() { LogImpl("", LogType::kPass); }

void TestFail(std::string_view msg) { LogImpl(msg, LogType::kFail); }

}  // namespace fw::testing
#include "fw/testing/test_suite.h"

#include <optional>
#include <string_view>

#include "fw/log/log.h"
#include "fw/testing/test_suite_interface.h"

namespace fw::testing {
namespace {

// TODO: Replace arbitrary headers with protobuf and serdes.
constexpr std::string_view kTestPrefix = "[FW_TEST]";
constexpr std::string_view kTestMsg = "<MSG>";
constexpr std::string_view kTestPass = "<PASS>";
constexpr std::string_view kTestFail = "<FAIL>";
constexpr std::string_view kTestStart = "<START>";
constexpr std::string_view kTestSuiteEnd = "<END>";

#define STRING_LOG(str) \
  LOG_BLOCKING("%.*s", static_cast<int>(str.length()), str.data());

// Sends a framed message over console to the harness.
void MessageImpl(std::string_view code,
                 std::optional<std::string_view> msg = std::nullopt) {
  STRING_LOG(kTestPrefix);
  STRING_LOG(code);
  if (msg.has_value()) {
    STRING_LOG(msg.value());
  }
  LOG_BLOCKING("\r\n");
}

}  // namespace

TestSuite::~TestSuite() {
  if (!ran_) {
    MessageImpl(kTestFail, "No tests ran in suite.");
    return;
  }
  MessageImpl(kTestSuiteEnd);
}

void TestSuite::Log(std::string_view msg) { MessageImpl(kTestMsg, msg); }

bool TestSuite::RunTest(std::string_view name,
                        std::function<void(TestSuiteInterface&)> test) {
  MessageImpl(kTestStart, name);
  test(*this);
  const bool result = !failure_;
  failure_ = false;
  ran_ = true;

  MessageImpl(result ? kTestPass : kTestFail);
  return result;
}

}  // namespace fw::testing
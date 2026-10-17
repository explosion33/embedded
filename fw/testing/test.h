#pragma once
#include <string_view>

namespace fw::testing {

// Logs a message to the test harness.
void LogTest(std::string_view msg);

// Passes the test.
void TestPass();

// Fails the test and logs the message.
void TestFail(std::string_view msg);

}  // namespace fw::testing
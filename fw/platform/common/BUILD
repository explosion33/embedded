load("@rules_cc//cc:cc_library.bzl", "cc_library")
load("@rules_cc//cc:cc_test.bzl", "cc_test")

package(default_visibility = ["//fw:__subpackages__"])

cc_library(
    name = "gpio_output_interface",
    hdrs = ["gpio_output_interface.h"],
)

cc_library(
    name = "gpio_input_interface",
    hdrs = ["gpio_input_interface.h"],
)

cc_library(
    name = "pin_lock",
    hdrs = ["pin_lock.h"],
    deps = [
        "//fw/log:check",
        "@magic_enum",
    ],
)

cc_test(
    name = "pin_lock_test",
    srcs = ["pin_lock_test.cc"],
    deps = [
        ":pin_lock",
        "//fw/log",
        "@googletest//:gtest_main",
    ],
)

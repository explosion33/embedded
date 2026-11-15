load("@rules_cc//cc:cc_library.bzl", "cc_library")

package(default_visibility = ["//fw/app:__subpackages__"])

cc_library(
    name = "peripheral_test_lib",
    srcs = ["peripheral_test.cc"],
    deps = [
        "//fw/os:thread",
        "//fw/os:time",
        "//fw/platform/stm32f4/common:freertos",
        "//fw/platform/stm32f4/common:gpio_input",
        "//fw/platform/stm32f4/common:gpio_output",
        "//fw/platform/stm32f4/common:interrupts",
        "//fw/testing:test",
    ],
    alwayslink = True,
)

load("@rules_cc//cc:cc_library.bzl", "cc_library")
load("//fw/platform/bazel:firmware.bzl", "cc_fw_app")

package(default_visibility = ["//fw:__subpackages__"])

cc_library(
    name = "test",
    srcs = ["test.cc"],
    hdrs = ["test.h"],
    deps = ["//fw/log"],
)

# TODO: Replace with a dedicated cc_fw_test once it exists.
cc_fw_app(
    name = "pass_test",
    srcs = ["pass_test.cc"],
    board = "//fw/boards/nucleo_f446re",
    deps = [
        "//fw/os:main",
        "//fw/platform/stm32f4/common:freertos",
        "//fw/testing:test",
    ],
)

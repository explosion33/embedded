load("@rules_cc//cc:cc_library.bzl", "cc_library")
load("@rules_python//python:py_binary.bzl", "py_binary")
load("//fw/platform/bazel:firmware.bzl", "cc_fw_app")
load("//fw/testing/bazel:firmware.bzl", "cc_fw_test")

package(default_visibility = ["//fw:__subpackages__"])

cc_library(
    name = "test",
    srcs = ["test.cc"],
    hdrs = ["test.h"],
    deps = ["//fw/log"],
)

cc_fw_test(
    name = "pass_test",
    srcs = ["pass_test.cc"],
    board = "//fw/boards/nucleo_f446re",
    target = "STM32F4",
    deps = [
        "//fw/os:main",
        "//fw/platform/stm32f4/common:freertos",
        "//fw/testing:test",
    ],
)

cc_fw_test(
    name = "fail_test",
    srcs = ["fail_test.cc"],
    board = "//fw/boards/nucleo_f446re",
    target = "STM32F4",
    deps = [
        "//fw/os:main",
        "//fw/platform/stm32f4/common:freertos",
        "//fw/testing:test",
    ],
)

py_binary(
    name = "run_test",
    srcs = ["run_test.py"],
    main = "run_test.py",
    visibility = ["//visibility:public"],
    deps = [
        "//fw/tools:flash",
        "@pypi//click",
        "@pypi//pyserial",
    ],
)

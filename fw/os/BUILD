load("@rules_cc//cc:cc_library.bzl", "cc_library")
load("//fw/testing/bazel:firmware.bzl", "cc_fw_test")

package(default_visibility = ["//fw:__subpackages__"])

cc_library(
    name = "thread",
    hdrs = ["thread.h"],
    target_compatible_with = ["@platforms//os:none"],
    deps = [
        ":main",  # Link main so os apps can't redifine main.
        ":semaphore",
        ":task_wrapper",
        "//fw/log:check",
    ],
)

cc_library(
    name = "time",
    srcs = ["time.cc"],
    hdrs = ["time.h"],
    target_compatible_with = ["@platforms//os:none"],
    deps = [
        ":main",  # Link main so os apps can't redifine main.
        "//fw/log:check",
        "@freertos_kernel//:freertos",
    ],
)

cc_library(
    name = "mutex",
    srcs = ["mutex.cc"],
    hdrs = ["mutex.h"],
    target_compatible_with = ["@platforms//os:none"],
    deps = [
        ":main",  # Link main so os apps can't redifine main.
        "//fw/log:check",
        "@freertos_kernel//:freertos",
    ],
)

cc_library(
    name = "semaphore",
    srcs = ["semaphore.cc"],
    hdrs = ["semaphore.h"],
    target_compatible_with = ["@platforms//os:none"],
    deps = [
        ":main",  # Link main so os apps can't redifine main.
        "//fw/log:check",
        "@freertos_kernel//:freertos",
    ],
)

cc_library(
    name = "task_wrapper",
    srcs = ["task_wrapper.cc"],
    hdrs = ["task_wrapper.h"],
    target_compatible_with = ["@platforms//os:none"],
    deps = [
        "//fw/log:check",
        "@freertos_kernel//:freertos",
    ],
)

cc_library(
    name = "main",
    srcs = ["main.cc"],
    hdrs = ["main.h"],
    implementation_deps = [
        "@freertos_kernel//:freertos",
    ],
    target_compatible_with = ["@platforms//os:none"],
    alwayslink = True,
)

cc_fw_test(
    name = "os_test",
    srcs = ["os_test.cc"],
    board = "//fw/boards/nucleo_f446re",
    target = "STM32F4",
    deps = [
        ":mutex",
        ":semaphore",
        ":thread",
        ":time",
        "//fw/platform/stm32f4/common:freertos",
        "//fw/testing:test_suite",
    ],
)

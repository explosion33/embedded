load("@rules_cc//cc:cc_library.bzl", "cc_library")
load("@rules_cc//cc:cc_test.bzl", "cc_test")
load("//fw/testing/bazel:firmware.bzl", "cc_fw_test")

package(default_visibility = ["//fw:__subpackages__"])

_FREERTOS = "@platforms//os:none"

cc_library(
    name = "thread",
    hdrs = ["thread.h"],
    deps = [
        ":semaphore",
        ":task_wrapper",
        "//fw/log:check",
    ] + select({
        _FREERTOS: [":main"],  # Link main so os apps can't redifine main.
        "//conditions:default": [],
    }),
)

cc_library(
    name = "time",
    srcs = select({
        _FREERTOS: ["time.cc"],
        "//conditions:default": ["host/time.cc"],
    }),
    hdrs = ["time.h"],
    deps = select({
        _FREERTOS: [
            ":main",  # Link main so os apps can't redifine main.
            "//fw/log:check",
            "@freertos_kernel//:freertos",
        ],
        "//conditions:default": [],
    }),
)

cc_test(
    name = "host_time_test",
    srcs = ["host/time_test.cc"],
    deps = [
        ":time",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "host_task_wrapper_test",
    srcs = ["host/task_wrapper_test.cc"],
    deps = [
        ":task_wrapper",
        ":time",
        "//fw/log",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "host_thread_test",
    srcs = ["host/thread_test.cc"],
    deps = [
        ":thread",
        ":time",
        "//fw/log",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "host_semaphore_test",
    srcs = ["host/semaphore_test.cc"],
    deps = [
        ":semaphore",
        ":thread",
        ":time",
        "//fw/log",
        "@googletest//:gtest_main",
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
    srcs = select({
        _FREERTOS: ["semaphore.cc"],
        "//conditions:default": ["host/semaphore.cc"],
    }),
    hdrs = ["semaphore.h"],
    deps = ["//fw/log:check"] + select({
        _FREERTOS: [
            ":main",  # Link main so os apps can't redifine main.
            "@freertos_kernel//:freertos",
        ],
        "//conditions:default": [],
    }),
)

cc_library(
    name = "task_wrapper",
    srcs = select({
        _FREERTOS: ["task_wrapper.cc"],
        "//conditions:default": ["host/task_wrapper.cc"],
    }),
    hdrs = ["task_wrapper.h"],
    deps = [
        "//fw/log:check",
    ] + select({
        _FREERTOS: ["@freertos_kernel//:freertos"],
        "//conditions:default": [],
    }),
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
        "//fw/testing:test",
    ],
)

load("@rules_cc//cc:cc_library.bzl", "cc_library")

package(default_visibility = ["//visibility:public"])

exports_files(["stm32f446re.ld"])

platform(
    name = "platform",
    constraint_values = [
        "@platforms//cpu:armv7",
        "@platforms//os:none",
        "//platform/constraints:cortex_m4f",
        "//platform/constraints:stm32f446",
    ],
)

cc_library(
    name = "f446",
    srcs = ["@cmsis_device_f4//:startup_stm32f446xx_src"],
    hdrs = ["platform.h"],
    target_compatible_with = ["//platform/constraints:stm32f446"],
    deps = [
        "//platform/stm32f4/common:crt_stubs",
        "//platform/stm32f4/common:ll",
        "//platform/stm32f4/common:uart",
    ],
    alwayslink = True,
)

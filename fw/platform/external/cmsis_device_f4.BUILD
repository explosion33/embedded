load("@rules_cc//cc:cc_library.bzl", "cc_library")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "headers",
    hdrs = glob(["Include/*.h"]),
    includes = ["Include"],
    deps = ["@cmsis_core//:headers"],
)

filegroup(
    name = "system_src",
    srcs = ["Source/Templates/system_stm32f4xx.c"],
)

filegroup(
    name = "startup_stm32f446xx_src",
    srcs = ["Source/Templates/gcc/startup_stm32f446xx.s"],
)

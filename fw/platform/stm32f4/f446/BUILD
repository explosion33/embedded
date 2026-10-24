load("@rules_cc//cc:cc_library.bzl", "cc_library")
load("//generators/stm32/pins:build_defs.bzl", "pin_map_from_stm32_data")

package(default_visibility = ["//visibility:public"])

exports_files(["stm32f446re.ld"])

cc_library(
    name = "freertos_config",
    hdrs = ["freertos/stm32f446_freertos_config.h"],
    target_compatible_with = ["//fw/platform/constraints:stm32f446"],
    deps = ["//fw/platform/stm32f4/common:freertos_config_stm32f4"],
)

cc_library(
    name = "f446",
    srcs = ["@cmsis_device_f4//:startup_stm32f446xx_src"],
    hdrs = ["platform.h"],
    target_compatible_with = ["//fw/platform/constraints:stm32f446"],
    deps = [
        ":stm32f446re_pin_map",
        "//fw/platform/stm32f4/common:crt_stubs",
        "//fw/platform/stm32f4/common:ll",
    ],
    alwayslink = True,
)

filegroup(
    name = "stm32f446re_chip_json",
    srcs = [
        "@STM32F446RE//file",
    ],
)

pin_map_from_stm32_data(
    name = "stm32f446re_pin_map",
    json_file = ":stm32f446re_chip_json",
    namespace = "fw::platform",
)

load("@rules_cc//cc:cc_library.bzl", "cc_library")
load("//generators/stm32/pins:build_defs.bzl", "pin_map_from_stm32_data")

package(default_visibility = ["//visibility:public"])

exports_files(["stm32f411cc.ld"])

cc_library(
    name = "freertos_config",
    hdrs = ["freertos/stm32f411_freertos_config.h"],
    target_compatible_with = ["//fw/platform/constraints:stm32f411"],
    deps = ["//fw/platform/stm32f4/common:freertos_config_stm32f4"],
)

cc_library(
    name = "f411",
    srcs = ["@cmsis_device_f4//:startup_stm32f411xe_src"],
    hdrs = ["platform.h"],
    implementation_deps = ["//fw/platform/stm32f4/common:ll"],
    target_compatible_with = ["//fw/platform/constraints:stm32f411"],
    deps = [
        ":stm32f411cc_pin_map",
        "//fw/platform/stm32f4/common:crt_stubs",
    ],
    alwayslink = True,
)

filegroup(
    name = "stm32f411cc_chip_json",
    srcs = [
        "@STM32F411CC//file",
    ],
)

pin_map_from_stm32_data(
    name = "stm32f411cc_pin_map",
    include_path = "fw/platform/stm32f4/common/pin_map.h",
    json_file = ":stm32f411cc_chip_json",
    namespace = "fw::platform::pins",
)

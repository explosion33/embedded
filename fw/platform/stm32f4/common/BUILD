load("@rules_cc//cc:cc_library.bzl", "cc_library")

package(default_visibility = ["//fw/platform/stm32f4:__subpackages__"])

# ST's drivers are third-party code we do not intend to fix warnings in.
VENDOR_COPTS = [
    "-Wno-unused-parameter",
    "-Wno-sign-compare",
]

# These libraries only make sense for a bare-metal target. Constraining them
# means `bazel build //...` on a host platform skips them instead of failing to
# find an ARM toolchain.
TARGET_COMPATIBLE_WITH = ["@platforms//os:none"]

cc_library(
    name = "cmsis",
    srcs = ["@cmsis_device_f4//:system_src"],
    copts = VENDOR_COPTS,
    defines = select(
        {
            "//fw/platform/constraints:is_stm32f446": ["STM32F446xx"],
        },
        no_match_error = "//fw/platform/stm32f4 requires an STM32F4 target platform",
    ),
    target_compatible_with = TARGET_COMPATIBLE_WITH,
    deps = ["@cmsis_device_f4//:headers"],
)

cc_library(
    name = "ll",
    srcs = [
        "@stm32f4xx_hal_driver//:ll_gpio_src",
        "@stm32f4xx_hal_driver//:ll_rcc_src",
        "@stm32f4xx_hal_driver//:ll_usart_src",
    ],
    copts = VENDOR_COPTS,
    defines = ["USE_FULL_LL_DRIVER"],
    target_compatible_with = TARGET_COMPATIBLE_WITH,
    visibility = ["//visibility:public"],
    deps = [
        ":cmsis",
        "@stm32f4xx_hal_driver//:ll_headers",
    ],
)

cc_library(
    name = "crt_stubs",
    srcs = ["crt_stubs.cc"],
    target_compatible_with = TARGET_COMPATIBLE_WITH,
    visibility = ["//visibility:public"],
    alwayslink = True,
)

cc_library(
    name = "uart",
    srcs = ["uart.cc"],
    hdrs = ["uart.h"],
    target_compatible_with = TARGET_COMPATIBLE_WITH,
    visibility = ["//visibility:public"],
    deps = [":ll"],
)

cc_library(
    name = "halt",
    srcs = ["halt.cc"],
    target_compatible_with = TARGET_COMPATIBLE_WITH,
    visibility = ["//visibility:public"],
    deps = [
        ":ll",
        "//fw/log:headers",
    ],
)

cc_library(
    name = "gpio_output",
    srcs = ["gpio_output.cc"],
    hdrs = ["gpio_output.h"],
    target_compatible_with = TARGET_COMPATIBLE_WITH,
    visibility = ["//visibility:public"],
    deps = [
        ":ll",
        "//fw/log:check",
        "//fw/platform/peripherals:gpio_output_interface",
        "//fw/platform/peripherals:pin_map",
    ],
)

cc_library(
    name = "freertos_config_stm32f4",
    hdrs = ["freertos/stm32f4_freertos_config.h"],
    target_compatible_with = TARGET_COMPATIBLE_WITH,
    deps = [":cmsis"],
)

cc_library(
    name = "freertos",
    srcs = [
        "freertos/freertos_heap.cc",
        "freertos/freertos_hooks.cc",
    ],
    target_compatible_with = TARGET_COMPATIBLE_WITH,
    visibility = ["//visibility:public"],
    deps = [
        "//fw/log",
        "@freertos_kernel//:freertos",
    ],
    alwayslink = True,
)

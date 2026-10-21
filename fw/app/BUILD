load("//fw/platform/bazel:firmware.bzl", "cc_fw_app")

cc_fw_app(
    name = "rtos_demo",
    srcs = ["rtos_demo.cc"],
    board = "//fw/boards/nucleo_f446re",
    deps = [
        "//fw/log",
        "//fw/os:thread",
        "//fw/os:time",
        "//fw/platform/stm32f4/common:freertos",
        "//fw/platform/stm32f4/common:gpio_output",
    ],
)

load("//fw/platform/bazel:firmware.bzl", "cc_fw_app")
load("//fw/testing/bazel:firmware.bzl", "cc_fw_test")

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

cc_fw_test(
    name = "peripheral_test",
    srcs = ["peripheral_test.cc"],
    board = "//fw/boards/nucleo_f446re",
    target = "stm32f446re",
    deps = [
        "//fw/os:thread",
        "//fw/os:time",
        "//fw/platform/stm32f4/common:freertos",
        "//fw/platform/stm32f4/common:gpio_input",
        "//fw/platform/stm32f4/common:gpio_output",
        "//fw/platform/stm32f4/common:interrupts",
        "//fw/testing:test",
    ],
)

cc_fw_test(
    name = "os_test",
    board = "//fw/boards/nucleo_f446re",
    target = "stm32f446re",
    deps = [
        "//fw/os:os_test_lib",
    ],
)

load("//fw/testing/bazel:firmware.bzl", "cc_fw_test")

cc_fw_test(
    name = "peripheral_test",
    board = "//fw/boards/blackpill",
    target = "stm32f411cc",
    deps = [
        "//fw/app/stm32f4:peripheral_test_lib",
    ],
)

cc_fw_test(
    name = "os_test",
    board = "//fw/boards/blackpill",
    target = "stm32f411cc",
    deps = [
        "//fw/os:os_test_lib",
        "//fw/platform/stm32f4/common:freertos",
    ],
)

cc_fw_test(
    name = "pass_test",
    board = "//fw/boards/blackpill",
    target = "stm32f411cc",
    deps = [
        "//fw/platform/stm32f4/common:freertos",
        "//fw/testing:pass_test_lib",
    ],
)

cc_fw_test(
    name = "fail_test",
    board = "//fw/boards/blackpill",
    negative = True,
    target = "stm32f411cc",
    deps = [
        # TODO: Here and other cc_fw_test should not directly link this. Boards should auto-link this.
        "//fw/platform/stm32f4/common:freertos",
        "//fw/testing:fail_test_lib",
    ],
)

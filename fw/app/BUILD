load("//fw/platform/bazel:firmware.bzl", "cc_fw_app")

cc_fw_app(
    name = "hello_world",
    srcs = ["hello_world.cc"],
    linker_script = "//fw/platform/stm32f4/f446:stm32f446re.ld",
    platform = "//fw/platform/stm32f4/f446:platform",
    deps = ["//fw/platform/stm32f4/f446"],
)

cc_fw_app(
    name = "rtos_demo",
    srcs = ["rtos_demo.cc"],
    linker_script = "//fw/platform/stm32f4/f446:stm32f446re.ld",
    platform = "//fw/platform/stm32f4/f446:platform",
    deps = [
        "//fw/log",
        "//fw/os:thread",
        "//fw/os:time",
        "//fw/platform/stm32f4/common:freertos",
        "//fw/platform/stm32f4/f446",
    ],
)

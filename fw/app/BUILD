load("//platform/bazel:firmware.bzl", "cc_fw_app")

cc_fw_app(
    name = "hello_world",
    srcs = ["hello_world.cc"],
    linker_script = "//platform/stm32f4/f446:stm32f446re.ld",
    target_compatible_with = ["//platform/constraints:stm32f446"],
    deps = ["//platform/stm32f4/f446"],
)

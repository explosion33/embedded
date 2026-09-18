load("@rules_cc//cc:cc_test.bzl", "cc_test")
load("@rules_python//python:py_test.bzl", "py_test")
load("//platform/bazel:firmware.bzl", "cc_fw_app")

py_test(
    name = "test_py",
    srcs = ["test.py"],
    main = "test.py",
)

cc_test(
    name = "test_cc",
    srcs = ["test.cc"],
    deps = [
        "@abseil-cpp//absl/status",
        "@googletest//:gtest_main",
    ],
)

cc_fw_app(
    name = "hello_world",
    srcs = ["hello_world.cc"],
    linker_script = "//platform/stm32f4/f446:stm32f446re.ld",
    target_compatible_with = ["//platform/constraints:stm32f446"],
    deps = ["//platform/stm32f4/f446"],
)

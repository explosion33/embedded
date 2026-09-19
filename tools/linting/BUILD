load("@rules_python//python:py_binary.bzl", "py_binary")

exports_files(["bazel_compile_commands.BUILD"])

py_binary(
    name = "generate_compile_commands",
    srcs = ["generate_compile_commands.py"],
    data = ["@bazel_compile_commands"],
    env = {
        "BAZEL_COMPILE_COMMANDS": "$(rlocationpath @bazel_compile_commands//:bazel_compile_commands)",
    },
    main = "generate_compile_commands.py",
    visibility = ["//visibility:public"],
    deps = [
        "@pypi//click",
        "@rules_python//python/runfiles",
    ],
)

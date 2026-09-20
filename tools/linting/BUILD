load("@rules_python//python:py_binary.bzl", "py_binary")
load("@rules_python//python/entry_points:py_console_script_binary.bzl", "py_console_script_binary")

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

py_binary(
    name = "lint",
    srcs = ["lint.py"],
    data = [
        "@pypi//ruff:data",
    ],
    env = {
        "RUFF": "$(rlocationpath @pypi//ruff:data)",
    },
    main = "lint.py",
    visibility = ["//visibility:public"],
    deps = [
        "@pypi//click",
        "@rules_python//python/runfiles",
    ],
)

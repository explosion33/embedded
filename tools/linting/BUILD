load("@rules_python//python:py_binary.bzl", "py_binary")
load("@rules_python//python:py_test.bzl", "py_test")
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

py_console_script_binary(
    name = "clang_format",
    pkg = "@pypi//clang_format",
    script = "clang-format",
)

py_console_script_binary(
    name = "pyright",
    pkg = "@pypi//pyright",
    script = "pyright",
)

py_binary(
    name = "lint",
    srcs = ["lint.py"],
    data = [
        ":clang_format",
        ":pyright",
        "@pypi//ruff:data",
    ],
    env = {
        "RUFF": "$(rlocationpath @pypi//ruff:data)",
        "PYRIGHT": "$(rlocationpath :pyright)",
        "CLANG_FORMAT": "$(rlocationpath :clang_format)",
    },
    main = "lint.py",
    visibility = ["//visibility:public"],
    deps = [
        "@pypi//click",
        "@pypi//gitignore_parser",
        "@rules_python//python/runfiles",
    ],
)

py_test(
    name = "lint_test",
    srcs = ["lint_test.py"],
    main = "lint_test.py",
    deps = [
        ":lint",
    ],
)

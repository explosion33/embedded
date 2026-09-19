load("@rules_python//python/entry_points:py_console_script_binary.bzl", "py_console_script_binary")
load("@rules_python//python/uv:lock.bzl", "lock")

# Pin clang-format through bazel instead of host to avoid machine-specific setup.
py_console_script_binary(
    name = "clang_format",
    pkg = "@pypi//clang_format",
    script = "clang-format",
)

# `bazel run //:uv.update` regenerates uv.lock in the source tree using the
# hermetic uv from MODULE.bazel.
lock(
    name = "uv",
    srcs = ["pyproject.toml"],
    out = "uv.lock",
)

alias(
    name = "generate_compile_commands",
    actual = "//tools/linting:generate_compile_commands",
)

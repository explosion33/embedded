load("@rules_python//python/uv:lock.bzl", "lock")

# `bazel run //:uv.update` regenerates uv.lock in the source tree using the
# hermetic uv from MODULE.bazel.
lock(
    name = "uv",
    srcs = ["pyproject.toml"],
    out = "uv.lock",
)

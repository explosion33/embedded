load("@rules_cc//cc:cc_library.bzl", "cc_library")

package(default_visibility = ["//fw:__subpackages__"])

cc_library(
    name = "log",
    srcs = ["log.cc"],
    deps = [
        ":backend",
        ":headers",
    ],
)

cc_library(
    name = "check",
    srcs = ["check.cc"],
    hdrs = ["check.h"],
    deps = [
        ":headers",
        ":log",
    ],
)

cc_library(
    name = "headers",
    hdrs = [
        "backend.h",
        "log.h",
    ],
)

label_flag(
    name = "backend",
    build_setting_default = ":no_backend",
)

# TODO: This can probably be defaulted to a host backend using std::cout.
cc_library(
    name = "no_backend",
    target_compatible_with = ["@platforms//:incompatible"],
)

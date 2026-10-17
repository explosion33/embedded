load("@rules_cc//cc:cc_library.bzl", "cc_library")
load("//fw/platform/bazel:firmware.bzl", "cc_fw_app")

package(default_visibility = ["//fw:__subpackages__"])

cc_library(
    name = "test",
    srcs = ["test.cc"],
    hdrs = ["test.h"],
    deps = ["//fw/log"],
)

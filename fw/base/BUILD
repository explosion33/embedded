load("@rules_cc//cc:cc_library.bzl", "cc_library")

package(default_visibility = ["//fw:__subpackages__"])

cc_library(
    name = "newlib_stubs",
    srcs = ["newlib_stubs.cc"],
    target_compatible_with = ["@platforms//os:none"],
    deps = ["//fw/log"],
    alwayslink = True,
)

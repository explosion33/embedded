load("@rules_cc//cc:cc_library.bzl", "cc_library")

package(default_visibility = ["//fw:__subpackages__"])

cc_library(
    name = "thread",
    hdrs = ["thread.h"],
    target_compatible_with = ["@platforms//os:none"],
    deps = [
        "@freertos_kernel//:freertos",
    ],
)

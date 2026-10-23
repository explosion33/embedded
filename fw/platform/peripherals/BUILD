load("@rules_cc//cc:cc_library.bzl", "cc_library")
load("//generators/pins:build_defs.bzl", "forward_pin_map")

package(default_visibility = ["//fw:__subpackages__"])

cc_library(
    name = "gpio_output_interface",
    hdrs = ["gpio_output_interface.h"],
)

forward_pin_map(
    name = "pin_map",
    namespace = "fw::platform",
)

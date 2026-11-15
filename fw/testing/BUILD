load("@rules_cc//cc:cc_library.bzl", "cc_library")
load("@rules_python//python:py_binary.bzl", "py_binary")

package(default_visibility = ["//fw:__subpackages__"])

cc_library(
    name = "test_suite",
    srcs = ["test_suite.cc"],
    hdrs = ["test_suite.h"],
    deps = [
        ":test_suite_interface",
        "//fw/log",
        "//fw/log:invocable_stream",
    ],
)

cc_library(
    name = "test_suite_interface",
    hdrs = ["test_suite_interface.h"],
    deps = [
        "//fw/log:invocable_stream",
    ],
)

cc_library(
    name = "test",
    srcs = ["test.cc"],
    hdrs = ["test.h"],
    deps = [
        ":test_suite",
        "//fw/log:failure_stream",
        "//fw/os:main",
    ],
)

cc_library(
    name = "pass_test_lib",
    srcs = ["pass_test.cc"],
    deps = [
        "//fw/os:main",
        "//fw/platform/stm32f4/common:freertos",
        "//fw/testing:test",
    ],
    alwayslink = True,
)

cc_library(
    name = "fail_test_lib",
    srcs = ["fail_test.cc"],
    deps = [
        "//fw/os:main",
        "//fw/platform/stm32f4/common:freertos",
        "//fw/testing:test",
    ],
    alwayslink = True,
)

py_binary(
    name = "run_test",
    srcs = ["run_test.py"],
    main = "run_test.py",
    visibility = ["//visibility:public"],
    deps = [
        "//fw/tools:flash",
        "@pypi//click",
        "@pypi//pyserial",
    ],
)

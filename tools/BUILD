load("@rules_cc//cc:cc_test.bzl", "cc_test")
load("@rules_python//python:py_test.bzl", "py_test")

py_test(
    name = "test_py",
    srcs = ["test.py"],
    main = "test.py",
)

cc_test(
    name = "test_cc",
    srcs = ["test.cc"],
    deps = [
        "@abseil-cpp//absl/status",
        "@googletest//:gtest_main",
    ],
)

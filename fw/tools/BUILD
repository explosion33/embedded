load("@rules_python//python:py_binary.bzl", "py_binary")
load("@rules_python//python:py_test.bzl", "py_test")

py_binary(
    name = "flash",
    srcs = ["flash.py"],
    main = "flash.py",
    visibility = ["//visibility:public"],
    deps = [
        "@pypi//click",
        "@pypi//pyocd",
    ],
)

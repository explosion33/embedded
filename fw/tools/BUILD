load("@rules_python//python:py_binary.bzl", "py_binary")
load("@rules_python//python:py_test.bzl", "py_test")

py_binary(
    name = "flash",
    srcs = ["flash.py"],
    main = "flash.py",
    data = ["@stm32f4xx_dfp//file"],
    visibility = ["//visibility:public"],
    deps = [
        "@pypi//click",
        "@pypi//pyocd",
        "@rules_python//python/runfiles",
    ],
)

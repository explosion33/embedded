load("@rules_cc//cc:cc_library.bzl", "cc_library")
load("@rules_cc//cc:cc_test.bzl", "cc_test")
load("@rules_python//python:py_binary.bzl", "py_binary")
load("@rules_python//python:py_test.bzl", "py_test")
load("//generators/stm32/pins:build_defs.bzl", "pin_map_from_stm32_data")

package(default_visibility = ["//generators/stm32/pins:__subpackages__"])

py_binary(
    name = "stm32_data_parser",
    srcs = ["stm32_data_parser.py"],
    main = "stm32_data_parser.py",
)

py_test(
    name = "stm32_data_parser_test",
    srcs = ["stm32_data_parser_test.py"],
    main = "stm32_data_parser_test.py",
    deps = [":stm32_data_parser"],
)

py_binary(
    name = "pin_map_from_stm32_data",
    srcs = ["pin_map_from_stm32_data.py"],
    main = "pin_map_from_stm32_data.py",
    deps = [
        ":stm32_data_parser",
        "@pypi//click",
        "@pypi//jinja2",
    ],
)

filegroup(
    name = "pin_map_h_template",
    srcs = ["templates/pin_map.h.j2"],
)

filegroup(
    name = "sample_stm32_data_json",
    srcs = ["test_data/sample_stm32_data.json"],
)

pin_map_from_stm32_data(
    name = "sample_stm32_data_pin_map",
    include_path = "generators/stm32/pins/sample_stm32_data_pin_map.h",
    json_file = ":sample_stm32_data_json",
    namespace = "generators::pins",
)

cc_test(
    name = "pin_map_from_stm32_data_test",
    srcs = ["pin_map_from_stm32_data_test.cc"],
    deps = [
        ":sample_stm32_data_pin_map",
        "@googletest//:gtest_main",
        "@magic_enum",
    ],
)

load("@rules_cc//cc:cc_library.bzl", "cc_library")
load("@rules_cc//cc:cc_test.bzl", "cc_test")
load("@rules_python//python:py_binary.bzl", "py_binary")
load("@rules_python//python:py_test.bzl", "py_test")
load("//generators/stm32/pins:build_defs.bzl", "forward_pin_map", "pin_map_from_stm32_xml")

package(default_visibility = ["//generators/stm32/pins:__subpackages__"])

py_binary(
    name = "stm32_xml_parser",
    srcs = ["stm32_xml_parser.py"],
    main = "stm32_xml_parser.py",
)

py_test(
    name = "stm32_xml_parser_test",
    srcs = ["stm32_xml_parser_test.py"],
    main = "stm32_xml_parser_test.py",
    deps = [":stm32_xml_parser"],
)

py_binary(
    name = "pin_map_from_stm32_xml",
    srcs = ["pin_map_from_stm32_xml.py"],
    main = "pin_map_from_stm32_xml.py",
    deps = [
        ":stm32_xml_parser",
        "@pypi//click",
        "@pypi//jinja2",
    ],
)

py_binary(
    name = "forward_pin_map",
    srcs = ["pin_map_foward.py"],
    main = "pin_map_foward.py",
    deps = [
        "@pypi//click",
        "@pypi//jinja2",
    ],
)

filegroup(
    name = "pin_map_h_template",
    srcs = ["templates/pin_map.h.j2"],
)

filegroup(
    name = "pin_map_cc_template",
    srcs = ["templates/pin_map.cc.j2"],
)

filegroup(
    name = "forward_pin_map_template",
    srcs = ["templates/pin_map_forward.h.j2"],
)

filegroup(
    name = "sample_stm32_xml",
    srcs = ["test_data/sample_stm32.xml"],
)

forward_pin_map(
    name = "pin_map",
    namespace = "generators::pins",
)

pin_map_from_stm32_xml(
    name = "sample_stm32_pin_map",
    namespace = "generators::pins",
    xml_file = ":sample_stm32_xml",
)

cc_library(
    name = "pin_map_from_stm32_xml_test_header",
    hdrs = ["pin_map_from_stm32_xml_test.h"],
)

cc_test(
    name = "pin_map_from_stm32_xml_test",
    srcs = ["pin_map_from_stm32_xml_test.cc"],
    deps = [
        ":pin_map",
        ":pin_map_from_stm32_xml_test_header",
        ":sample_stm32_pin_map",
        "@googletest//:gtest_main",
        "@magic_enum",
    ],
)

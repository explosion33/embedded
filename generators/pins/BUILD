load("@rules_cc//cc:cc_test.bzl", "cc_test")
load("@rules_python//python:py_binary.bzl", "py_binary")
load("@rules_python//python:py_test.bzl", "py_test")
load("//generators/pins:build_defs.bzl", "pin_map_from_stm32_xml")

package(default_visibility = ["//generators/pins:__subpackages__"])

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

filegroup(
    name = "pin_map_template",
    srcs = ["templates/pin_map.h.j2"],
)

filegroup(
    name = "sample_stm32_xml",
    srcs = ["test_data/sample_stm32.xml"],
)

pin_map_from_stm32_xml(
    name = "sample_stm32_pin_map",
    namespace = "generators::pins",
    xml_file = ":sample_stm32_xml",
)

cc_test(
    name = "pin_map_from_stm32_xml_test",
    srcs = ["pin_map_from_stm32_xml_test.cc"],
    deps = [
        ":sample_stm32_pin_map",
        "@googletest//:gtest_main",
        "@magic_enum",
    ],
)

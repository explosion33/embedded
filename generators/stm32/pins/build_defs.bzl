load("@rules_cc//cc:cc_library.bzl", "cc_library")

_PIN_MAP_H_TEMPLATE = Label("//generators/stm32/pins:pin_map_h_template")
_PIN_MAP_CC_TEMPLATE = Label("//generators/stm32/pins:pin_map_cc_template")
_FORWARD_PIN_MAP_TEMPLATE = Label("//generators/stm32/pins:forward_pin_map_template")
_STM32_DATA_TO_PIN_MAP = "//generators/stm32/pins:pin_map_from_stm32_data"
_FORWARD_PIN_MAP = "//generators/stm32/pins:forward_pin_map"

def _pin_map_from_stm32_data_impl(
        name,
        visibility,
        json_file,
        namespace,
        include_path):
    include_dir, _, header_name = include_path.rpartition("/")

    # Generate into its own directory so that prefix can be swapped for
    # `include_dir`. Symbolic macros require outputs be prefixed by `<name>_`.
    output_dir = name + "_include"
    output_h = "{}/{}".format(output_dir, header_name)
    output_cc = name + ".cc"

    cmd = " ".join([
        "$(location {})".format(_STM32_DATA_TO_PIN_MAP),
        "--path $(location {})".format(json_file),
        "--template_h $(location {})".format(_PIN_MAP_H_TEMPLATE),
        "--template_cc $(location {})".format(_PIN_MAP_CC_TEMPLATE),
        "--output_h $(location {})".format(output_h),
        "--output_cc $(location {})".format(output_cc),
        "--namespace {}".format(namespace),
        "--header_path {}".format(include_path),
    ])

    native.genrule(
        name = name + "_header",
        srcs = [_PIN_MAP_H_TEMPLATE, _PIN_MAP_CC_TEMPLATE, json_file],
        outs = [output_h, output_cc],
        cmd = cmd,
        tools = [_STM32_DATA_TO_PIN_MAP],
    )

    cc_library(
        name = name,
        hdrs = [output_h],
        srcs = [output_cc],
        strip_include_prefix = output_dir,
        include_prefix = include_dir,
        visibility = visibility,
    )

pin_map_from_stm32_data = macro(
    doc = """Generate a C++ Pin Map file from an stm32-data chip JSON file.

    Emits a header, included as ``include_path``, containing a Pin and Port enum
    for each parsed pin, as well as helper functions surrounding pin
    capabilities, and enum parsing.
    """,
    implementation = _pin_map_from_stm32_data_impl,
    attrs = {
        "json_file": attr.string(
            doc = "A bazel file target pointing to the stm32-data chip JSON file to parse.",
            mandatory = True,
            configurable = False,
        ),
        "namespace": attr.string(
            doc = "The C++ namespace the generated mapping code is under.",
            mandatory = True,
            configurable = False,
        ),
        "include_path": attr.string(
            doc = """The path the generated header is included by from its consumers.""",
            mandatory = True,
            configurable = False,
        ),
    },
)

def _forward_pin_map_impl(
        name,
        visibility,
        namespace):
    output_file = name + ".h"

    cmd = " ".join([
        "$(location {})".format(_FORWARD_PIN_MAP),
        "--template $(location {})".format(_FORWARD_PIN_MAP_TEMPLATE),
        "--output $@",
        "--namespace {}".format(namespace),
    ])

    native.genrule(
        name = name + "_header",
        srcs = [_FORWARD_PIN_MAP_TEMPLATE],
        outs = [output_file],
        cmd = cmd,
        tools = [_FORWARD_PIN_MAP],
    )

    cc_library(
        name = name,
        hdrs = [output_file],
        visibility = visibility,
    )

forward_pin_map = macro(
    doc = """Generate a C++ forward declared Pin Map file.
    
    Emits a header ``<name>.h`` containing a Pin and Port enum definition and
    definitions for helper functions. Does not include implementations.
    """,
    implementation = _forward_pin_map_impl,
    attrs = {
        "namespace": attr.string(
            doc = "The C++ namespace the generated mapping code is under.",
            mandatory = True,
            configurable = False,
        ),
    },
)

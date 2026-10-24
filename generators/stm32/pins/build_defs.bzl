load("@rules_cc//cc:cc_library.bzl", "cc_library")

_PIN_MAP_H_TEMPLATE = Label("//generators/stm32/pins:pin_map_h_template")
_PIN_MAP_CC_TEMPLATE = Label("//generators/stm32/pins:pin_map_cc_template")
_FORWARD_PIN_MAP_TEMPLATE = Label("//generators/stm32/pins:forward_pin_map_template")
_STM32_PIN_TO_PIN_MAP = "//generators/stm32/pins:pin_map_from_stm32_xml"
_FORWARD_PIN_MAP = "//generators/stm32/pins:forward_pin_map"

def _pin_map_from_stm32_xml_impl(
        name,
        visibility,
        xml_file,
        namespace):
    output_h = name + ".h"
    output_cc = name + ".cc"
    header_include = "{}/{}".format(native.package_name(), output_h)

    cmd = " ".join([
        "$(location {})".format(_STM32_PIN_TO_PIN_MAP),
        "--path $(location {})".format(xml_file),
        "--template_h $(location {})".format(_PIN_MAP_H_TEMPLATE),
        "--template_cc $(location {})".format(_PIN_MAP_CC_TEMPLATE),
        "--output_h $(location {})".format(output_h),
        "--output_cc $(location {})".format(output_cc),
        "--namespace {}".format(namespace),
        "--header_path {}".format(header_include),
    ])

    native.genrule(
        name = name + "_header",
        srcs = [_PIN_MAP_H_TEMPLATE, _PIN_MAP_CC_TEMPLATE, xml_file],
        outs = [output_h, output_cc],
        cmd = cmd,
        tools = [_STM32_PIN_TO_PIN_MAP],
    )

    cc_library(
        name = name,
        hdrs = [output_h],
        srcs = [output_cc],
        visibility = visibility,
    )

pin_map_from_stm32_xml = macro(
    doc = """Generate a C++ Pin Map file from an STM32 XML Target / Board mapping.
    
    Emits a header ``<name>.h`` containing a Pin and Port enum for each parsed pin,
    as well as helper functions surroungding pin capabilities, and enum parsing.
    """,
    implementation = _pin_map_from_stm32_xml_impl,
    attrs = {
        "xml_file": attr.string(
            doc = "A bazel file target pointing to the STM32 XML file to parse.",
            mandatory = True,
            configurable = False,
        ),
        "namespace": attr.string(
            doc = "The C++ namespace the generated mapping code is under.",
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

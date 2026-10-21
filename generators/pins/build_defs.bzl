load("@rules_cc//cc:cc_library.bzl", "cc_library")

_PIN_MAP_TEMPLATE = Label("//generators/pins:pin_map_template")
_STM32_PIN_TO_PIN_MAP = "//generators/pins:pin_map_from_stm32_xml"

def _pin_map_from_stm32_xml_impl(
        name,
        visibility,
        xml_file,
        namespace):
    output_file = name + ".h"

    cmd = " ".join([
        "$(location {})".format(_STM32_PIN_TO_PIN_MAP),
        "--path $(location {})".format(xml_file),
        "--template $(location {})".format(_PIN_MAP_TEMPLATE),
        "--output $@",
        "--namespace {}".format(namespace),
    ])

    native.genrule(
        name = name + "_header",
        srcs = [_PIN_MAP_TEMPLATE, xml_file],
        outs = [output_file],
        cmd = cmd,
        tools = [_STM32_PIN_TO_PIN_MAP],
    )

    cc_library(
        name = name,
        hdrs = [output_file],
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

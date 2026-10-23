load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_file")

_pin_file_tag = tag_class(
    attrs = {
        "file_name": attr.string(mandatory = True),
        "sha256": attr.string(mandatory = False),
    },
)

def _stm32_pins_impl(ctx):
    for mod in ctx.modules:
        for pin_tag in mod.tags.download:
            repo_name = pin_tag.file_name.replace(".xml", "").replace("(", "_").replace(")", "_").replace("-", "_")

            raw_url = "https://raw.githubusercontent.com/STMicroelectronics/STM32_open_pin_data/7d1f1514ed5583ec5007ad91236b4e1d377295b1/mcu/{}".format(pin_tag.file_name)

            http_file(
                name = repo_name,
                urls = [raw_url],
                sha256 = pin_tag.sha256,
            )

stm32_pins = module_extension(
    implementation = _stm32_pins_impl,
    tag_classes = {"download": _pin_file_tag},
)

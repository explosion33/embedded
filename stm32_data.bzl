load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_file")

_STM32_DATA_COMMIT = "2f16f70371ce3e0fb6b444532a6dd88a796cea8c"

_chip_tag = tag_class(
    attrs = {
        "chip": attr.string(mandatory = True),
        "sha256": attr.string(mandatory = False),
    },
)

def _stm32_data_impl(ctx):
    for mod in ctx.modules:
        for chip_tag in mod.tags.download:
            raw_url = "https://raw.githubusercontent.com/embassy-rs/stm32-data-generated/{}/data/chips/{}.json".format(
                _STM32_DATA_COMMIT,
                chip_tag.chip,
            )

            http_file(
                name = chip_tag.chip,
                urls = [raw_url],
                sha256 = chip_tag.sha256,
            )

stm32_data = module_extension(
    implementation = _stm32_data_impl,
    tag_classes = {"download": _chip_tag},
)

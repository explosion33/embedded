load("@rules_cc//cc:action_names.bzl", "ACTION_NAMES")
load("@rules_cc//cc:cc_binary.bzl", "cc_binary")
load("@rules_cc//cc:find_cc_toolchain.bzl", "find_cc_toolchain", "use_cc_toolchain")
load("@rules_cc//cc/common:cc_common.bzl", "cc_common")

def _objcopy_binary_impl(ctx):
    cc_toolchain = find_cc_toolchain(ctx)
    feature_configuration = cc_common.configure_features(
        ctx = ctx,
        cc_toolchain = cc_toolchain,
        requested_features = ctx.features,
        unsupported_features = ctx.disabled_features,
    )
    objcopy = cc_common.get_tool_for_action(
        feature_configuration = feature_configuration,
        action_name = ACTION_NAMES.objcopy_embed_data,
    )

    elf = ctx.file.src
    out = ctx.actions.declare_file(ctx.label.name)

    ctx.actions.run(
        executable = objcopy,
        arguments = ["-O", ctx.attr.format, elf.path, out.path],
        inputs = depset(
            direct = [elf],
            transitive = [cc_toolchain.all_files],
        ),
        outputs = [out],
        mnemonic = "Objcopy",
        progress_message = "Converting %{input} to " + ctx.attr.format,
    )

    return [DefaultInfo(files = depset([out]))]

_objcopy_binary = rule(
    implementation = _objcopy_binary_impl,
    doc = "Converts a linked ELF into a flat image using the toolchain's objcopy.",
    attrs = {
        "src": attr.label(
            allow_single_file = True,
            mandatory = True,
            doc = "The linked .elf to convert.",
        ),
        "format": attr.string(
            default = "binary",
            doc = "An objcopy output format, passed to -O.",
        ),
    },
    fragments = ["cpp"],
    toolchains = use_cc_toolchain(),
)

def _platform_transition_impl(settings, attr):
    _ = settings
    return {"//command_line_option:platforms": [str(attr.platform)]}

_platform_transition = transition(
    implementation = _platform_transition_impl,
    inputs = [],
    outputs = ["//command_line_option:platforms"],
)

def _fw_image_impl(ctx):
    return [DefaultInfo(files = depset(
        transitive = [src[DefaultInfo].files for src in ctx.attr.srcs],
    ))]

_fw_image = rule(
    implementation = _fw_image_impl,
    doc = "Builds source files for its labeled platform",
    attrs = {
        "srcs": attr.label_list(
            cfg = _platform_transition,
            mandatory = True,
            doc = "The outputs to build for `platform`.",
        ),
        "platform": attr.label(
            mandatory = True,
            doc = "The platform to build `srcs` for.",
        ),
    },
)

def cc_fw_app(
        name,
        linker_script,
        platform,
        srcs = [],
        deps = [],
        linkopts = [],
        target_compatible_with = ["@platforms//os:none"],
        **kwargs):
    """Builds a flashable firmware image. Produces .bin .elf and .map files.

    Args:
      name: Name of the firmware image.
      linker_script: The linker script describing the part's memory map.
      platform: The platform to cross compile this app for.
      srcs: C/C++/assembly sources.
      deps: cc_library/binary dependencies for this app.
      linkopts: Extra linker flags.
      target_compatible_with: Platforms the .elf and .bin can be built for.
      **kwargs: Args passed through to the underyling binary.
    """
    elf_name = name + ".elf"

    cc_binary(
        name = elf_name,
        srcs = srcs,
        deps = deps,
        additional_linker_inputs = [linker_script],
        features = ["generate_linkmap"],
        linkopts = [
            "-T$(execpath %s)" % linker_script,
        ] + linkopts,
        target_compatible_with = target_compatible_with,
        **kwargs
    )

    _objcopy_binary(
        name = name + ".bin",
        src = ":" + elf_name,
        format = "binary",
        target_compatible_with = target_compatible_with,
    )

    native.filegroup(
        name = name + ".map",
        srcs = [":" + elf_name],
        output_group = "linkmap",
    )

    _fw_image(
        name = name,
        srcs = [
            ":" + elf_name,
            ":" + name + ".bin",
            ":" + name + ".map",
        ],
        platform = platform,
    )

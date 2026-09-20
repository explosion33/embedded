load("@rules_cc//cc/toolchains:args.bzl", "cc_args")
load("@rules_cc//cc/toolchains:tool.bzl", "cc_tool")
load("@rules_cc//cc/toolchains:tool_map.bzl", "cc_tool_map")
load("@rules_cc//cc/toolchains:toolchain.bzl", "cc_toolchain")

package(default_visibility = ["//visibility:private"])

exports_files(["external.BUILD"])

# Tools --------------------------------------------------------------------

cc_tool(
    name = "gcc",
    src = "@arm_none_eabi//:bin/arm-none-eabi-gcc",
    data = ["@arm_none_eabi//:all_files"],
    tags = ["manual"],
)

cc_tool(
    name = "gpp",
    src = "@arm_none_eabi//:bin/arm-none-eabi-g++",
    data = ["@arm_none_eabi//:all_files"],
    tags = ["manual"],
)

cc_tool(
    name = "ld",
    src = "@arm_none_eabi//:bin/arm-none-eabi-g++",
    data = ["@arm_none_eabi//:all_files"],
    tags = ["manual"],
)

cc_tool(
    name = "ar",
    src = "@arm_none_eabi//:bin/arm-none-eabi-ar",
    data = ["@arm_none_eabi//:all_files"],
    tags = ["manual"],
)

cc_tool(
    name = "objcopy",
    src = "@arm_none_eabi//:bin/arm-none-eabi-objcopy",
    data = ["@arm_none_eabi//:all_files"],
    tags = ["manual"],
)

cc_tool(
    name = "strip",
    src = "@arm_none_eabi//:bin/arm-none-eabi-strip",
    data = ["@arm_none_eabi//:all_files"],
    tags = ["manual"],
)

cc_tool_map(
    name = "tools",
    tags = ["manual"],
    tools = {
        "@rules_cc//cc/toolchains/actions:ar_actions": ":ar",
        # ST ships its startup code as hand-written .s, so assembly actions
        # must be mapped or firmware will not link.
        "@rules_cc//cc/toolchains/actions:assembly_actions": ":gcc",
        "@rules_cc//cc/toolchains/actions:c_compile_actions": ":gcc",
        "@rules_cc//cc/toolchains/actions:cpp_compile_actions": ":gpp",
        "@rules_cc//cc/toolchains/actions:link_actions": ":ld",
        "@rules_cc//cc/toolchains/actions:objcopy_embed_data": ":objcopy",
        "@rules_cc//cc/toolchains/actions:strip": ":strip",
    },
)

# Arguments ----------------------------------------------------------------

cc_args(
    name = "core_args",
    actions = [
        "@rules_cc//cc/toolchains/actions:compile_actions",
        "@rules_cc//cc/toolchains/actions:link_actions",
        "@rules_cc//cc/toolchains/actions:assembly_actions",
    ],
    args = select({
        "//fw/platform/constraints:cortex_m4f": [
            "-mcpu=cortex-m4",
            "-mthumb",
            "-mfpu=fpv4-sp-d16",
            "-mfloat-abi=hard",
        ],
    }),
    tags = ["manual"],
)

cc_args(
    name = "relative_path_args",
    actions = [
        "@rules_cc//cc/toolchains/actions:compile_actions",
        "@rules_cc//cc/toolchains/actions:assembly_actions",
    ],
    args = [
        "-no-canonical-prefixes",
        "-fno-canonical-system-headers",
    ],
    tags = ["manual"],
)

cc_args(
    name = "no_runtime_args",
    actions = ["@rules_cc//cc/toolchains/actions:cpp_compile_actions"],
    args = [
        "-fno-exceptions",
        "-fno-rtti",
        "-fno-threadsafe-statics",
        "-fno-unwind-tables",
    ],
    tags = ["manual"],
)

cc_args(
    name = "section_args",
    actions = [
        "@rules_cc//cc/toolchains/actions:compile_actions",
        "@rules_cc//cc/toolchains/actions:assembly_actions",
    ],
    args = [
        "-ffunction-sections",
        "-fdata-sections",
    ],
    tags = ["manual"],
)

cc_args(
    name = "link_args",
    actions = ["@rules_cc//cc/toolchains/actions:link_actions"],
    args = [
        "-nostartfiles",
        "--specs=nano.specs",
        "--specs=nosys.specs",
        "-Wl,--gc-sections",
        "-Wl,--print-memory-usage",
    ],
    tags = ["manual"],
)

# Toolchain ----------------------------------------------------------------

cc_toolchain(
    name = "arm_none_eabi",
    args = [
        ":core_args",
        ":relative_path_args",
        ":no_runtime_args",
        ":section_args",
        ":link_args",
    ],
    compiler = "gcc",
    enabled_features = [
        "@rules_cc//cc/toolchains/args:experimental_replace_legacy_action_config_features",
    ],
    known_features = ["@rules_cc//cc/toolchains/args/generate_linkmap:feature"],
    tags = ["manual"],
    tool_map = ":tools",
    visibility = ["//visibility:public"],
)

toolchain(
    name = "arm_none_eabi_toolchain",
    tags = ["manual"],
    target_compatible_with = [
        "@platforms//cpu:armv7",
        "@platforms//os:none",
    ],
    toolchain = ":arm_none_eabi",
    toolchain_type = "@rules_cc//cc:toolchain_type",
    visibility = ["//visibility:public"],
)

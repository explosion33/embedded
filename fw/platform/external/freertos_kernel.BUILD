load("@rules_cc//cc:cc_library.bzl", "cc_library")

package(default_visibility = ["//visibility:public"])

# The kernel #includes "FreeRTOSConfig.h" by name, so whoever builds it must
# supply that header. Platforms point this flag at their config library
# through `platform(flags = ...)`.
label_flag(
    name = "config",
    build_setting_default = ":no_config",
)

# The default is incompatible with everything: the kernel cannot be built
# without a platform choosing a config.
cc_library(
    name = "no_config",
    target_compatible_with = ["@platforms//:incompatible"],
)

# FreeRTOS is third-party code we do not intend to fix warnings in.
VENDOR_COPTS = [
    "-Wno-unused-parameter",
    "-Wno-sign-compare",
]

cc_library(
    name = "port",
    srcs = select(
        {
            "@//fw/platform/constraints:cortex_m4f": ["portable/GCC/ARM_CM4F/port.c"],
        },
        no_match_error = "FreeRTOS has no port configured for this core",
    ),
    hdrs = select({
        "@//fw/platform/constraints:cortex_m4f": ["portable/GCC/ARM_CM4F/portmacro.h"],
    }),
    copts = VENDOR_COPTS,
    includes = select({
        "@//fw/platform/constraints:cortex_m4f": ["portable/GCC/ARM_CM4F"],
    }),
    deps = [":headers"],
)

cc_library(
    name = "headers",
    hdrs = glob(["include/*.h"]),
    includes = ["include"],
    deps = [":config"],
)

cc_library(
    name = "freertos",
    srcs = [
        "event_groups.c",
        "list.c",
        "queue.c",
        "stream_buffer.c",
        "tasks.c",
        "timers.c",
        "portable/MemMang/heap_4.c",
    ],
    copts = VENDOR_COPTS,
    target_compatible_with = ["@platforms//os:none"],
    deps = [
        ":headers",
        ":port",
    ],
)

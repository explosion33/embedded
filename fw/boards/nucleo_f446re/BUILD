load("@rules_cc//cc:cc_library.bzl", "cc_library")

package(default_visibility = ["//visibility:public"])

# The memory map apps on this board are linked with.
LINKER_SCRIPT = "//fw/platform/stm32f4/f446:stm32f446re.ld"

platform(
    name = "platform",
    constraint_values = [
        "@platforms//cpu:armv7",
        "@platforms//os:none",
        "//fw/platform/constraints:cortex_m4f",
        "//fw/platform/constraints:stm32f446",
    ],
    flags = [
        "--@freertos_kernel//:config=//fw/boards/nucleo_f446re:freertos_config",
        "--//fw/log:backend=//fw/boards/nucleo_f446re:log_backend",
    ],
)

# The board library every app on this board links. Carries the part's startup
# code and the board's linker script.
cc_library(
    name = "nucleo_f446re",
    srcs = ["board.cc"],
    hdrs = ["board.h"],
    additional_linker_inputs = [LINKER_SCRIPT],
    linkopts = ["-T$(execpath %s)" % LINKER_SCRIPT],
    target_compatible_with = ["//fw/platform/constraints:stm32f446"],
    deps = [
        "//fw/platform/stm32f4/common:gpio_output",
        "//fw/platform/stm32f4/common:ll",
        "//fw/platform/stm32f4/common:uart",
        "//fw/platform/stm32f4/f446",
    ],
)

# Provides FreeRTOSConfig.h to the FreeRTOS kernel, via the flag set above.
cc_library(
    name = "freertos_config",
    hdrs = ["FreeRTOSConfig.h"],
    # The kernel includes it as "FreeRTOSConfig.h", not by its full path.
    strip_include_prefix = ".",
    target_compatible_with = ["//fw/platform/constraints:stm32f446"],
    deps = ["//fw/platform/stm32f4/f446:freertos_config"],
)

# The //fw/log backend: writes to the console UART and halts the Cortex-M.
cc_library(
    name = "log_backend",
    srcs = ["log_write.cc"],
    target_compatible_with = ["//fw/platform/constraints:stm32f446"],
    deps = [
        ":nucleo_f446re",
        "//fw/log:headers",
        "//fw/platform/stm32f4/common:halt",
    ],
)

load("@rules_cc//cc:cc_library.bzl", "cc_library")

package(default_visibility = ["//visibility:public"])

# Every LL module that ships a .c file. Most LL functionality is inline in the
# headers; these sources hold the LL_*_Init()/LL_*_DeInit() helpers and the
# clock-tree queries, and only compile at all under USE_FULL_LL_DRIVER.
LL_MODULES = [
    "adc",
    "crc",
    "dac",
    "dma",
    "dma2d",
    "exti",
    "fmc",
    "fmpi2c",
    "fsmc",
    "gpio",
    "i2c",
    "lptim",
    "pwr",
    "rcc",
    "rng",
    "rtc",
    "sdmmc",
    "spi",
    "tim",
    "usart",
    "usb",
    "utils",
]

cc_library(
    name = "ll_headers",
    hdrs = glob(["Inc/stm32f4xx_ll_*.h"]),
    includes = ["Inc"],
    deps = ["@cmsis_device_f4//:headers"],
)

[
    filegroup(
        name = "ll_%s_src" % module,
        srcs = ["Src/stm32f4xx_ll_%s.c" % module],
    )
    for module in LL_MODULES
]

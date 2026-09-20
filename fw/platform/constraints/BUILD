package(default_visibility = ["//visibility:public"])

# Defines the CPU core constraints.
constraint_setting(name = "core")

constraint_value(
    name = "cortex_m4f",
    constraint_setting = ":core",
)

# Defines the MCU  constraints.
constraint_setting(name = "mcu")

constraint_value(
    name = "stm32f446",
    constraint_setting = ":mcu",
)

config_setting(
    name = "is_stm32f446",
    constraint_values = [":stm32f446"],
)

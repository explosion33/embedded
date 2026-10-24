import dataclasses
import pathlib
import re

import click
import jinja2

from generators.stm32.pins import stm32_data_parser

PORT_PIN_REGEX = re.compile(r"^P(?P<port>\w)(?P<pin>\d+).*$")


@dataclasses.dataclass
class Pin:
    enum_name: str
    full_name: str
    output: bool
    port_enum: str
    port: str
    pin_idx: int

    @staticmethod
    def from_data(pin: stm32_data_parser.Stm32Data.GpioPin) -> Pin:
        match = re.search(PORT_PIN_REGEX, pin.name)
        if not match:
            raise click.ClickException(f"Got unexpected pin name: '{pin.name}'")
        port = match.group("port")
        pin_idx = int(match.group("pin"))

        return Pin(
            enum_name=f"kP{port.capitalize()}{pin_idx}",
            full_name=pin.name,
            # stm32-data does not track per-pin IO modes. Every STM32 GPIO can be
            # configured as an output, though some (ex. PC13-15) have reduced drive.
            output=True,
            port_enum=f"k{port.capitalize()}",
            port=port.capitalize(),
            pin_idx=pin_idx,
        )


@dataclasses.dataclass
class EnumValue:
    enum_name: str
    enum_value: int


@click.command()
@click.option("--path", required=True, type=pathlib.Path)
@click.option("--template_h", required=True, type=pathlib.Path)
@click.option("--template_cc", required=True, type=pathlib.Path)
@click.option("--output_h", required=True, type=pathlib.Path)
@click.option("--output_cc", required=True, type=pathlib.Path)
@click.option("--namespace", required=True, type=str)
@click.option("--header_path", required=True, type=str)
def main(
    path: pathlib.Path,
    template_h: pathlib.Path,
    template_cc: pathlib.Path,
    output_h: pathlib.Path,
    output_cc: pathlib.Path,
    namespace: str,
    header_path: str,
):
    data = stm32_data_parser.Stm32Data(path)

    if template_h.parent != template_cc.parent:
        raise ValueError("Templates must be in same directroy.")

    env = jinja2.Environment(
        loader=jinja2.FileSystemLoader(template_h.parent), trim_blocks=True, lstrip_blocks=True
    )
    j2_h_template = env.get_template(template_h.name)
    j2_cc_template = env.get_template(template_cc.name)

    pins = sorted(
        [Pin.from_data(pin) for pin in data.get_gpio_pins()], key=lambda pin: pin.full_name
    )
    ports = sorted(list(set(pin.port for pin in pins)))
    for port in ports:
        if len(port) > 1 or ord(port[0]) < ord("A") or ord(port[0]) > ord("Z"):
            raise click.ClickException(f"Got unsupported port name {port}")
    port_enums = [EnumValue(f"k{port}", ord(port[0]) - ord("A")) for port in ports]
    content_h = j2_h_template.render(
        namespace=namespace,
        pins=pins,
        ports=port_enums,
    )
    output_h.write_text(content_h)

    content_cc = j2_cc_template.render(
        header_path=header_path,
        namespace=namespace,
        pins=pins,
        ports=ports,
    )
    output_cc.write_text(content_cc)


if __name__ == "__main__":
    main()

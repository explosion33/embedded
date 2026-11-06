import dataclasses
import pathlib
import re

import click
import jinja2

from generators.stm32.pins import stm32_data_parser

PORT_PIN_REGEX = re.compile(r"^P(?P<port>\w)(?P<pin>\d+).*$")
UART_REGEX = re.compile(r"^U(S|)ART(?P<idx>\d+)$")


def get_enum_name(pin_name: str):
    """Gets the enum name for a given pin name."""
    match = re.search(PORT_PIN_REGEX, pin_name)
    if not match:
        raise click.ClickException(f"Got unexpected pin name: '{pin.name}'")
    port = match.group("port")
    pin_idx = int(match.group("pin"))
    return f"kP{port.capitalize()}{pin_idx}"


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
            enum_name=get_enum_name(pin.name),
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


@dataclasses.dataclass
class Uart:
    enum: EnumValue
    tx_pins: list[EnumValue]  # value is AF.
    rx_pins: list[EnumValue]  # value is AF.


def _parse_uarts(data: stm32_data_parser.Stm32Data) -> list[Uart]:
    """Parses all UARTs and USARTs into their enum equivelents and pin lists."""
    uarts: list[Uart] = []
    for uart in data.get_peripherals("usart") + data.get_peripherals("uart"):
        match = re.search(UART_REGEX, uart.name)
        if not match:
            raise click.ClickException(f"Got unexpected uart name: '{uart.name}'")
        idx = int(match.group("idx"))
        entry = Uart(EnumValue(f"k{uart.name}", idx), [], [])

        for pin in uart.pins:
            if pin.signal == "TX":
                entry.tx_pins.append(EnumValue(get_enum_name(pin.pin), pin.af))
            if pin.signal == "RX":
                entry.rx_pins.append(EnumValue(get_enum_name(pin.pin), pin.af))

        if len(entry.tx_pins) == 0:
            raise click.ClickException(f"Expected at least 1 TX pin for {uart.name}, got 0.")

        if len(entry.rx_pins) == 0:
            raise click.ClickException(f"Expected at least 1 RX pin for {uart.name}, got 0.")

        uarts.append(entry)

    return uarts


def _parse_pins(data: stm32_data_parser.Stm32Data) -> tuple[list[Pin], list[EnumValue]]:
    """Parses the raw data pins into a listt of Pin objects, and a list of Port enums."""

    pins = sorted(
        [Pin.from_data(pin) for pin in data.get_gpio_pins()], key=lambda pin: pin.full_name
    )
    ports = sorted(list(set(pin.port for pin in pins)))
    for port in ports:
        if len(port) > 1 or ord(port[0]) < ord("A") or ord(port[0]) > ord("Z"):
            raise click.ClickException(f"Got unsupported port name {port}")
    port_enums = [EnumValue(f"k{port}", ord(port[0]) - ord("A")) for port in ports]
    return (pins, port_enums)


@click.command()
@click.option("--path", required=True, type=pathlib.Path)
@click.option("--template_h", required=True, type=pathlib.Path)
@click.option("--output_h", required=True, type=pathlib.Path)
@click.option("--namespace", required=True, type=str)
def main(
    path: pathlib.Path,
    template_h: pathlib.Path,
    output_h: pathlib.Path,
    namespace: str,
):
    data = stm32_data_parser.Stm32Data(path)

    env = jinja2.Environment(
        loader=jinja2.FileSystemLoader(template_h.parent), trim_blocks=True, lstrip_blocks=True
    )
    j2_h_template = env.get_template(template_h.name)

    data = stm32_data_parser.Stm32Data(path)
    t_pins, t_ports = _parse_pins(data)
    uarts = _parse_uarts(data)

    content_h = j2_h_template.render(
        namespace=namespace,
        pins=t_pins,
        ports=t_ports,
        uarts=uarts,
    )
    output_h.write_text(content_h)


if __name__ == "__main__":
    main()

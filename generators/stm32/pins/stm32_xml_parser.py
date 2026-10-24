import dataclasses
import pathlib
import re
from enum import Enum
from xml.etree import ElementTree


class Stm32Xml:
    """
    Parses an STM32 MCU XML File, like the ones defined in STM32_open_pin_data.
    https://github.com/STMicroelectronics/STM32_open_pin_data/tree/master/mcu
    """

    @dataclasses.dataclass
    class Usart:
        class PinType(Enum):
            TX = 0
            RX = 1

        index: int
        pin_type: PinType

    # TODO: Expand this parsing / definition of a pin as more data is needed.
    @dataclasses.dataclass
    class GpioPin:
        """Defines a pin, and its supported peripherals / modes."""

        name: str
        output: bool
        usarts: list[Stm32Xml.Usart]

    @staticmethod
    def _get_namespace(root: ElementTree.Element[str]) -> str | None:
        """Gets the defined namespace if any for an xml tree."""
        match = re.match(r"\{.*\}", root.tag)
        return match.group(0) if match else None

    def __init__(self, xml_path: pathlib.Path):
        self.root = ElementTree.parse(xml_path).getroot()
        self.ns = Stm32Xml._get_namespace(self.root)

    def _with_ns(self, tag: str):
        """Gets the tag string with its associated namespace if any."""
        if not self.ns:
            return tag
        return f"{self.ns}{tag}"

    def _has_output(self, pin: ElementTree.Element[str], name: str) -> bool:
        gpio_signals = [
            signal
            for signal in pin.findall(self._with_ns("Signal"))
            if signal.get("Name") == "GPIO"
        ]
        if len(gpio_signals) == 0:
            return False
        if len(gpio_signals) > 1:
            raise ValueError(f"Parsed more than one gpio signal for pin '{name}'")
        modes = gpio_signals[0].get("IOModes")
        if not modes:
            raise ValueError(f"Parsed I/O pin '{name}' with no modes")
        return "Output" in modes

    def _get_usarts(self, pin: ElementTree.Element[str], pin_name: str) -> list[Stm32Xml.Usart]:
        usart_names = [
            name
            for signal in pin.findall(self._with_ns("Signal"))
            if (name := signal.get("Name", "")) and name.startswith("USART")
        ]

        out: list[Stm32Xml.Usart] = []
        for name in usart_names:
            if not (name.endswith("TX") or name.endswith("RX")):
                continue
            match = re.match(r"^USART(\d+)_.+$", name)
            if not match or match.group(1) is None:
                raise ValueError(f"Received malformed Uart name: '{name}'")
            idx = int(match.group(1))
            pin_type = (
                Stm32Xml.Usart.PinType.RX if name.endswith("RX") else Stm32Xml.Usart.PinType.TX
            )
            out.append(Stm32Xml.Usart(index=idx, pin_type=pin_type))

        return out

    def get_gpio_pins(self) -> list[GpioPin]:
        """Gets a list of `GpioPin` objects parsed from the provided XML root."""
        out: list[Stm32Xml.GpioPin] = []

        names: set[str] = set()
        for pin in self.root.findall(self._with_ns("Pin")):
            if pin.get("Type") != "I/O":
                continue
            name = pin.get("Name")
            if not name:
                raise ValueError("Parsed I/O pin with no name")
            if name in names:
                raise ValueError(f"Found duplicate pin definition for '{name}'")
            names.add(name)

            out.append(
                Stm32Xml.GpioPin(
                    name=name,
                    output=self._has_output(pin, name),
                    usarts=self._get_usarts(pin, name),
                )
            )

        return out

import dataclasses
import pathlib
import re
from xml.etree import ElementTree


class Stm32Xml:
    """
    Parses an STM32 MCU XML File, like the ones defined in STM32_open_pin_data.
    https://github.com/STMicroelectronics/STM32_open_pin_data/tree/master/mcu
    """

    # TODO: Expand this parsing / definition of a pin as more data is needed.
    @dataclasses.dataclass
    class GpioPin:
        """Defines a pin, and its supported peripherals / modes."""

        name: str
        output: bool

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

    def get_gpio_pins(self) -> list[GpioPin]:
        """Gets a list of `GpioPin` objects parsed from the provided XML root."""
        out: list[Stm32Xml.GpioPin] = []

        for pin in self.root.findall(self._with_ns("Pin")):
            if pin.get("Type") != "I/O":
                continue
            name = pin.get("Name")
            if not name:
                raise ValueError("Parsed I/O pin with no name")

            gpio_signals = [
                signal
                for signal in pin.findall(self._with_ns("Signal"))
                if signal.get("Name") == "GPIO"
            ]
            if len(gpio_signals) == 0:
                continue
            if len(gpio_signals) > 1:
                raise ValueError(f"Parsed more than one gpio signal for pin '{name}'")
            modes = gpio_signals[0].get("IOModes")
            if not modes:
                raise ValueError(f"Parsed I/O pin '{name}' with no modes")
            out.append(Stm32Xml.GpioPin(name=name, output="Output" in modes))

        return out

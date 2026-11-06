import dataclasses
import json
import pathlib
from typing import Any


class Stm32Data:
    """
    Parses an STM32 chip JSON file, like the ones defined in stm32-data-generated.
    https://github.com/embassy-rs/stm32-data-generated/tree/main/data/chips
    """

    @dataclasses.dataclass
    class GpioPin:
        """Defines a GPIO pin available on the chip."""

        name: str

    @dataclasses.dataclass
    class PeripheralPin:
        """A pin that can be routed to a peripheral signal through an alternate function."""

        pin: str
        signal: str
        af: int

    @dataclasses.dataclass
    class Peripheral:
        """Defines a peripheral instance, and the pins it can be routed to."""

        name: str
        kind: str
        pins: list[Stm32Data.PeripheralPin]

    def __init__(self, json_path: pathlib.Path):
        self.root = json.loads(json_path.read_text())
        self.core = self._get_core()

    def _get_core(self) -> dict[str, Any]:
        """Gets the single core defined for this chip."""
        cores = self.root.get("cores")
        if not cores:
            raise ValueError("Parsed chip with no cores")
        # TODO: Support multi-core chips by selecting a core by name.
        if len(cores) > 1:
            raise ValueError(f"Parsed chip with {len(cores)} cores, only 1 is supported")
        return cores[0]

    @staticmethod
    def _parse_peripheral_pin(pin: dict[str, Any], peripheral: str) -> Stm32Data.PeripheralPin:
        name = pin.get("pin")
        signal = pin.get("signal")
        af = pin.get("af")
        if not name or not signal:
            raise ValueError(f"Parsed malformed pin on peripheral '{peripheral}': {pin}")
        if not isinstance(af, int):
            raise ValueError(f"Parsed pin '{name}' on peripheral '{peripheral}' with no AF")
        return Stm32Data.PeripheralPin(pin=name, signal=signal, af=af)

    def get_gpio_pins(self) -> list[GpioPin]:
        """Gets a list of `GpioPin` objects parsed from the chip's core."""
        out: list[Stm32Data.GpioPin] = []

        names: set[str] = set()
        for pin in self.core.get("pins", []):
            name = pin.get("name")
            if not name:
                raise ValueError("Parsed pin with no name")
            if name in names:
                raise ValueError(f"Found duplicate pin definition for '{name}'")
            names.add(name)
            out.append(Stm32Data.GpioPin(name=name))

        return out

    def get_peripherals(self, kind: str | None = None) -> list[Peripheral]:
        """
        Gets a list of `Peripheral` objects parsed from the chip's core, optionally
        filtered to a register `kind` (ex. "usart", "spi").
        """
        out: list[Stm32Data.Peripheral] = []

        for peripheral in self.core.get("peripherals", []):
            name = peripheral.get("name")
            if not name:
                raise ValueError("Parsed peripheral with no name")
            # Peripherals without registers (ex. core / debug blocks) have no kind.
            peripheral_kind = peripheral.get("registers", {}).get("kind")
            if peripheral_kind is None or (kind is not None and peripheral_kind != kind):
                continue
            out.append(
                Stm32Data.Peripheral(
                    name=name,
                    kind=peripheral_kind,
                    pins=[
                        Stm32Data._parse_peripheral_pin(pin, name)
                        for pin in peripheral.get("pins", [])
                    ],
                )
            )

        return out

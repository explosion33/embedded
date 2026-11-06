import json
import pathlib
import tempfile
import unittest
from typing import Any

from generators.stm32.pins import stm32_data_parser


def _construct_with(data: dict[str, Any]) -> stm32_data_parser.Stm32Data:
    """Constructs a JSON parser with the provided chip data."""
    with tempfile.NamedTemporaryFile(mode="w+t", delete_on_close=False) as file:
        json.dump(data, file)
        file.close()
        return stm32_data_parser.Stm32Data(pathlib.Path(file.name))


def _with_core(core: dict[str, Any]) -> stm32_data_parser.Stm32Data:
    """Constructs a JSON parser with a single core."""
    return _construct_with({"name": "STM32TEST", "cores": [core]})


class TestStm32Data(unittest.TestCase):
    def test_cores(self):
        with self.assertRaises(ValueError):
            _construct_with({"name": "STM32TEST"})

        with self.assertRaises(ValueError):
            _construct_with({"name": "STM32TEST", "cores": []})

        with self.assertRaises(ValueError):
            _construct_with({"name": "STM32TEST", "cores": [{}, {}]})

    def test_get_gpio_pins(self):
        data = _with_core({"pins": [{"name": "PA0"}, {"name": "PC14"}]})
        self.assertCountEqual(
            data.get_gpio_pins(),
            [
                stm32_data_parser.Stm32Data.GpioPin(name="PA0"),
                stm32_data_parser.Stm32Data.GpioPin(name="PC14"),
            ],
        )

        self.assertEqual(_with_core({}).get_gpio_pins(), [])

    def test_get_gpio_pins_malformed(self):
        with self.assertRaises(ValueError):
            _with_core({"pins": [{}]}).get_gpio_pins()

        with self.assertRaises(ValueError):
            _with_core({"pins": [{"name": "PA0"}, {"name": "PA0"}]}).get_gpio_pins()

    def test_get_peripherals(self):
        data = _with_core(
            {
                "peripherals": [
                    {
                        "name": "USART1",
                        "registers": {"kind": "usart"},
                        "pins": [
                            {"pin": "PA9", "signal": "TX", "af": 7},
                            {"pin": "PA10", "signal": "RX", "af": 7},
                        ],
                    },
                    {"name": "GPIOA", "registers": {"kind": "gpio"}},
                    {"name": "DBGMCU"},
                ]
            }
        )

        usart1 = stm32_data_parser.Stm32Data.Peripheral(
            name="USART1",
            kind="usart",
            pins=[
                stm32_data_parser.Stm32Data.PeripheralPin(pin="PA9", signal="TX", af=7),
                stm32_data_parser.Stm32Data.PeripheralPin(pin="PA10", signal="RX", af=7),
            ],
        )
        gpioa = stm32_data_parser.Stm32Data.Peripheral(name="GPIOA", kind="gpio", pins=[])

        self.assertCountEqual(data.get_peripherals(), [usart1, gpioa])
        self.assertEqual(data.get_peripherals("usart"), [usart1])
        self.assertEqual(data.get_peripherals("spi"), [])

    def test_get_peripherals_malformed(self):
        with self.assertRaises(ValueError):
            _with_core({"peripherals": [{"registers": {"kind": "usart"}}]}).get_peripherals()

        for pin in [
            {"signal": "TX", "af": 7},
            {"pin": "PA9", "af": 7},
            {"pin": "PA9", "signal": "TX"},
            {"pin": "PA9", "signal": "TX", "af": "7"},
        ]:
            with self.subTest(pin=pin), self.assertRaises(ValueError):
                _with_core(
                    {
                        "peripherals": [
                            {"name": "USART1", "registers": {"kind": "usart"}, "pins": [pin]}
                        ]
                    }
                ).get_peripherals()


if __name__ == "__main__":
    unittest.main()

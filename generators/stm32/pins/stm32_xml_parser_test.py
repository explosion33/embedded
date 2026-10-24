import pathlib
import tempfile
import unittest

from generators.stm32.pins import stm32_xml_parser

Usart = stm32_xml_parser.Stm32Xml.Usart


def _construct_with(text: str) -> stm32_xml_parser.Stm32Xml:
    """Constructs a Xml parser with the provided text. text starts at root."""
    with tempfile.NamedTemporaryFile(mode="w+t", delete_on_close=False) as file:
        file.write("""<?xml version="1.0" encoding="UTF-8" standalone="no"?>""")
        file.write(text)
        file.close()
        return stm32_xml_parser.Stm32Xml(pathlib.Path(file.name))


class TestStm32Xml(unittest.TestCase):
    def test_get_namespace(self):
        xml = _construct_with("""
        <Root xmlns="namespace">
        </Root>
        """)
        self.assertEqual(stm32_xml_parser.Stm32Xml._get_namespace(xml.root), "{namespace}")

    def test_with_namespace(self):
        xml = _construct_with("""
                <Root xmlns="https://my_namespace">
                </Root>
                """)
        self.assertEqual(xml._with_ns("tag"), "{https://my_namespace}tag")

        no_ns_xml = _construct_with("""
                        <DiffRoot>
                        </DiffRoot>
                        """)
        self.assertEqual(no_ns_xml._with_ns("tag"), "tag")

    def test_get_gpio_pins(self):
        xml = _construct_with("""
                <Mcu>
                    <Pin Name="PA0" Type="I/O">
                        <Signal Name="USART2_TX"/>
                        <Signal Name="USART4_RX"/>
                        <Signal Name="USART2_OTHER_PIN"/>
                        <Signal IOModes="Output" Name="GPIO"/>
                    </Pin>
                    <Pin Name="PA2" Type="I/O">
                        <Signal Name="Other"/>
                    </Pin>
                    <Pin Name="PA3" Type="Other"/>
                    <Pin Name="PA1-Suffix" Type="I/O">
                        <Signal Name="Other"/>
                        <Signal IOModes="Input" Name="GPIO"/>
                    </Pin>
                </Mcu>
                """)
        print(xml.get_gpio_pins())
        self.assertCountEqual(
            xml.get_gpio_pins(),
            [
                stm32_xml_parser.Stm32Xml.GpioPin(
                    name="PA0",
                    output=True,
                    usarts=[
                        stm32_xml_parser.Stm32Xml.Usart(2, Usart.PinType.TX),
                        stm32_xml_parser.Stm32Xml.Usart(4, Usart.PinType.RX),
                    ],
                ),
                stm32_xml_parser.Stm32Xml.GpioPin(name="PA2", output=False, usarts=[]),
                stm32_xml_parser.Stm32Xml.GpioPin(name="PA1-Suffix", output=False, usarts=[]),
            ],
        )

    def test_get_gpio_pins_malformed(self):
        with self.assertRaises(ValueError):
            _construct_with("""
                <Mcu>
                    <Pin Type="I/O"/>
                </Mcu>
                """).get_gpio_pins()

        with self.assertRaises(ValueError):
            _construct_with("""
                <Mcu>
                    <Pin Name="name" Type="I/O">
                        <Signal IOModes="Input" Name="GPIO"/>
                        <Signal IOModes="Input" Name="GPIO"/>
                    </Pin>
                </Mcu>
                """).get_gpio_pins()

        with self.assertRaises(ValueError):
            _construct_with("""
                <Mcu>
                    <Pin Name="name" Type="I/O">
                        <Signal Name="GPIO"/>
                    </Pin>
                </Mcu>
                """).get_gpio_pins()


if __name__ == "__main__":
    unittest.main()

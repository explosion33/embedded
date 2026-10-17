import pathlib
import sys

import serial

from fw.tools import flash

TEST_FILE = pathlib.Path("bazel-out/k8-fastbuild-ST-a8fd264bfc15/bin/fw/testing/fail_test.bin")
BOARD = "STM32F4"

# Must match values defined in fw/testing/test.cc
TEST_PREFIX = "[FW_TEST]"
TEST_SUFFIX = "[END_TEST]"
TEST_PASS = "<PASS>"
TEST_FAIL = "<FAIL>"


def _get_text(text: str, prefix: str, suffix: str | None = None) -> str | None:
    """Gets the text inbetween the provided prefix / suffix."""

    start = text.find(prefix)
    if start == -1:
        return None

    start_idx = start + len(prefix)

    if suffix is None:
        return text[start_idx:]

    end = text.find(suffix, start)
    if end == -1:
        return None

    return text[start_idx:end]


def main():
    with serial.Serial("/dev/ttyACM0", 115200, timeout=1) as ser:
        flash.flash_from(TEST_FILE, BOARD)

        text = ""
        while True:
            text += ser.readline().decode("utf-8").strip()

            if msg := _get_text(text, TEST_PREFIX, TEST_SUFFIX):
                if TEST_PASS in msg:
                    print("Test Passed!")
                    sys.exit()
                elif fail_msg := _get_text(msg, TEST_FAIL):
                    print("Test Failed!")
                    print(fail_msg)
                    sys.exit(1)
                else:
                    print(msg)
                text = ""


if __name__ == "__main__":
    main()

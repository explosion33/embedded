import pathlib
import sys

import click
import serial
from python.runfiles import runfiles  # pyright: ignore[reportMissingImports]

from fw.tools import flash

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


@click.command
@click.option(
    "--binary_rlocation",
    required=True,
    type=str,
)
@click.option(
    "--target",
    required=True,
    type=str,
    help="The target of the device to autodetect and flash too.",
)
@click.option(
    "--negative", required=False, type=bool, default=False, help="Passes the test on test failure."
)
def main(binary_rlocation: str, target: str, negative: bool):
    pass_code = 1 if negative else 0
    fail_code = 0 if negative else 1

    binary_path = runfiles.Create().Rlocation(binary_rlocation)
    if binary_path is None:
        raise click.ClickException(f"Could not find '{binary_rlocation}' in runfiles.")

    with serial.Serial("/dev/ttyACM0", 115200, timeout=1) as ser:
        flash.flash_from(pathlib.Path(binary_path), target)

        text = ""
        while True:
            text += ser.readline().decode("utf-8").strip()

            if msg := _get_text(text, TEST_PREFIX, TEST_SUFFIX):
                if TEST_PASS in msg:
                    print("Test Passed!")
                    sys.exit(pass_code)
                elif fail_msg := _get_text(msg, TEST_FAIL):
                    print("Test Failed!")
                    print(fail_msg)
                    sys.exit(fail_code)
                else:
                    print(msg)
                text = ""


if __name__ == "__main__":
    main()

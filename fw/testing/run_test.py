import contextlib
import enum
import pathlib
import sys

import click
import serial
from python.runfiles import runfiles  # pyright: ignore[reportMissingImports]

from fw.tools import flash

# Must match values defined in fw/testing/test.cc
TEST_PREFIX = "[FW_TEST]"
TEST_MSG = "<MSG>"
TEST_PASS = "<PASS>"
TEST_FAIL = "<FAIL>"
TEST_START = "<START>"
TEST_END = "<END>"


class TerminalColor(enum.Enum):
    RED = 0
    GREEN = 1
    GREY = 2


@contextlib.contextmanager
def terminal_color(color: TerminalColor):
    """Applies ANSI color codes to the test within the context"""

    def _get_code(color: TerminalColor) -> str:
        if color == TerminalColor.RED:
            return "31"
        elif color == TerminalColor.GREEN:
            return "32"
        elif color == TerminalColor.GREY:
            return "38;2;120;120;120"
        raise ValueError("Unreachable")

    sys.stdout.write(f"\033[{_get_code(color)}m")
    sys.stdout.flush()
    try:
        yield
    finally:
        sys.stdout.write("\033[0m")
        sys.stdout.flush()


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


def error(msg: str):
    """
    Performs an exit for non-python related errors. A standard exception
    includes a large stack trace which clutters the terminal.
    """
    with terminal_color(TerminalColor.GREY):
        print(msg)
    sys.exit(1)


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
    binary_path = runfiles.Create().Rlocation(binary_rlocation)
    if binary_path is None:
        raise click.ClickException(f"Could not find '{binary_rlocation}' in runfiles.")

    with serial.Serial("/dev/ttyACM0", 115200, timeout=1) as ser:
        with terminal_color(TerminalColor.GREY):
            flash.flash_from(pathlib.Path(binary_path), target)

        test_names = []
        test_statuses = []
        while True:
            line = ser.readline().decode("utf-8").strip()
            if msg := _get_text(line, TEST_PREFIX):
                if TEST_PASS in msg:
                    test_statuses.append(True)
                    if len(test_statuses) != len(test_names):
                        error("Received status before a test was started.")
                    print("[ ", end="")
                    with terminal_color(TerminalColor.GREEN):
                        print("PASS", end="")
                    print(" ]")
                elif TEST_FAIL in msg:
                    test_statuses.append(False)
                    if len(test_statuses) != len(test_names):
                        error("Received status before a test was started.")
                    print("[ ", end="")
                    with terminal_color(TerminalColor.RED):
                        print("FAIL", end="")
                    print(" ]")
                elif msg_text := _get_text(msg, TEST_MSG):
                    print(f"\t{msg_text}")
                elif test_name := _get_text(msg, TEST_START):
                    if len(test_names) != len(test_statuses):
                        error(
                            f"Cannot start new test, current test {test_names[-1]} has not finished"
                        )
                    test_names.append(test_name)
                    print(f"[ {test_name} ]")
                elif TEST_END in msg:
                    break
                else:
                    error(f"Got malformed message: {msg}")
            elif line:
                with terminal_color(TerminalColor.GREY):
                    print(line)

        if len(test_statuses) == 0:
            error("Received no test results, but got test end.")

        longest_len = 0
        for name in test_names:
            if len(name) > longest_len:
                longest_len = len(name)

        print()
        print("=" * longest_len * 5)
        for name, status in zip(test_names, test_statuses, strict=True):
            print(f"[ {name} ]".ljust(longest_len + 5), end="")
            with terminal_color(TerminalColor.GREEN if status else TerminalColor.RED):
                print("PASS" if status else "FAIL")

        count = 0
        for status in test_statuses:
            if status:
                count += 1
        print(f"{count} / {len(test_statuses)} passed")

        all_passed = count == len(test_statuses)
        all_failed = count == 0

        if not negative:
            if all_passed:
                sys.exit(0)
            sys.exit(1)
        if all_failed:
            sys.exit(0)
        sys.exit(1)


if __name__ == "__main__":
    main()

import dataclasses
import pathlib

import click
from pyocd.core import helpers, session
from pyocd.flash.file_programmer import FileProgrammer
from pyocd.probe import debug_probe


def get_default_base(target: str) -> int | None:
    """
    Returns the default starting address for common families. Or None if one
    isn't recorded. i.e.:
    get_default_base("STM32F446RE") -> 0x08000000
    get_default_base("STM32F401CE") -> 0x08000000
    """
    known_bases = {"STM32F4": 0x08000000}

    for prefix, base in known_bases.items():
        if target.upper().startswith(prefix):
            return base
    return None


@dataclasses.dataclass
class ProbeInfo:
    uid: str
    description: str
    name: str
    target: str
    probe: debug_probe.DebugProbe


def get_probes_with_family(family: str) -> list[ProbeInfo]:
    """Gets a list of  `ProbeInfo` objects that match the given family.
    Searches all connected probes and finds devices with a similar family.
    Family compatibily is not garunteed and must be handled downstream. e.g.
    "STM32F4" -> (STM32F401CC, STM32F446RE)
    "STM32F446" -> (STM32F446RE)
    """
    return [
        ProbeInfo(
            uid=probe.unique_id,
            description=probe.description,
            name=info.name,
            target=target,
            probe=probe,
        )
        for probe in helpers.ConnectHelper.get_all_connected_probes()
        if (info := probe.associated_board_info)
        and (target := info.target)
        and target.upper().startswith(family.upper())
    ]


def get_probe(uid: str) -> debug_probe.DebugProbe:
    """Gets a probe reference for the probe with the provided uid"""
    for probe in helpers.ConnectHelper.get_all_connected_probes():
        if probe.unique_id == uid:
            return probe
    raise ValueError(f"Debug Probe with UID: {uid} does not exist.")


@dataclasses.dataclass
class FlashConfig:
    probe: debug_probe.DebugProbe
    binary: pathlib.Path
    flash_address: int
    target: str
    reset: bool = True
    verbose: bool = False

    def __str__(self):
        return (
            "FlashConfig {"
            f"\n\tbinary: '{self.binary!s}'"
            f"\n\taddress: 0x{self.flash_address:08X}"
            f"\n\ttarget: {self.target}"
            f"\n\treset: {self.reset}"
            "\n}"
        )


def flash(config: FlashConfig):
    if config.verbose:
        print(f"Connection to DebugProbe with config: {config}")

    probe_session = session.Session(config.probe, target_override=config.target)
    if probe_session is None:
        raise RuntimeError("Could not connect to provided debug probe.")

    with probe_session:
        board = probe_session.board
        if board is None:
            raise RuntimeError("Session has no board")

        target = board.target
        if config.verbose:
            print(f"Connected to probe with target {target.part_number}")

        programmer = FileProgrammer(probe_session)

        if config.verbose:
            print("Flashing binary")
        programmer.program(str(config.binary), base_address=config.flash_address)

        if config.verbose:
            print("Flashing Complete")

        if config.reset:
            if config.verbose:
                print("Resetting Target")
            target.reset()


@click.command()
@click.argument(
    "binary_path",
    required=True,
    type=pathlib.Path,
)
@click.option(
    "--target",
    required=True,
    type=str,
    help="The target of the device to autodetect and flash too.",
)
def main(binary_path: pathlib.Path, target: str):
    """
    Flashes the binary at the provided path to the first debug probe found that
    matches the provided target.
    """
    probe_infos = get_probes_with_family(target)

    if not probe_infos:
        raise click.ClickException(
            f"No Debug Probes found for target: '{target}', please specify full target and UID."
        )
    info = probe_infos[0]

    base_address = get_default_base(info.target)
    if base_address is None:
        raise click.ClickException(
            f"Could not detect base address for target '{target}', please explicitely provide one."
        )

    flash(
        FlashConfig(
            probe=info.probe,
            binary=binary_path,
            flash_address=base_address,
            target=info.target,
            reset=True,
            verbose=True,
        )
    )

    # assert_reset
    # reset


if __name__ == "__main__":
    main()

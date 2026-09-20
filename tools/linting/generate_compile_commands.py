"""Generates the compile_commands.json that clangd reads.

uv run generate_compile_commands //path/to:target
uv run generate_compile_commands //... --clean

Entries accumulate into the json by default so generating sub-targets does
not override existing targets. Use `--clean` to override the existing json.
"""

import collections
import json
import os
import pathlib
import re
import subprocess
import tempfile
import xml.etree.ElementTree as ET

import click
from python.runfiles import runfiles  # pyright: ignore[reportMissingImports]

# Get the workspace directoy provided by `bazel run`.
WORKSPACE = pathlib.Path(os.environ.get("BUILD_WORKSPACE_DIRECTORY", ""))

# Symlinked directory that holds external bazel builds for path stability.
WORKSPACE_EXTERNAL_CACHE = pathlib.Path("external")

# Rule kinds that compile C++.
CC_RULE_KINDS = (
    "cc_binary",
    "cc_import",
    "cc_library",
    "cc_shared_library",
    "cc_static_library",
    "cc_test",
)


def echo(message: str) -> None:
    """Stylized click echo wrapper."""
    click.echo(f"{click.style('==>', fg='cyan')} {message}", err=True)


def bazel(*args: str) -> str:
    """Runs bazel in the WORKSPACE"""
    done = subprocess.run(
        ["bazel", *args],
        cwd=WORKSPACE,
        text=True,
        capture_output=True,
        check=False,
    )
    if done.returncode != 0:
        raise click.ClickException(f"bazel {' '.join(args)} failed:\n{done.stderr}")
    return done.stdout


def platform_configs() -> dict[str, frozenset[str]]:
    """Maps each platform-selecting --config in .bazelrc to its constraints."""

    platform_config_re = re.compile(r"^(?:build|common):([\w.-]+)\s+--platforms[=\s]+(\S+)")

    configs = {}
    for line in (WORKSPACE / ".bazelrc").read_text().splitlines():
        match = platform_config_re.match(line.strip())
        if match:
            name, platform = match.groups()
            constraints = bazel(
                "query",
                f"labels(constraint_values, {platform})",
                "--output=label",
            )
            configs[name] = frozenset(constraints.split())
    return configs


def declared_constraints(patterns: list[str]) -> dict[str, frozenset[str]]:
    """Queries all bazel C++ targets from the given patterns and maps each to their cosntraints."""
    query = " + ".join(patterns)
    kinds = "|".join(CC_RULE_KINDS)
    document = ET.fromstring(
        bazel("query", f'kind("^({kinds}) rule$", deps({query}))', "--output=xml")
    )
    return {
        name: frozenset(
            value
            for attribute in rule.findall("list[@name='target_compatible_with']")
            for label in attribute
            if (value := label.get("value"))
        )
        for rule in document.findall("rule")
        # Filter out external C++ files listed by query.
        if (name := rule.get("name", "")) and name.startswith("//")
    }


def group_by_config(
    targets: dict[str, frozenset[str]], configs: dict[str, frozenset[str]]
) -> dict[str | None, list[str]]:
    """Maps a mapping of targets to constraints to a mapping of config to targets."""
    groups = collections.defaultdict(list)
    for target, constraints in sorted(targets.items()):
        if not constraints:
            groups[None].append(target)
            continue

        matches = [name for name, satisfied in configs.items() if constraints <= satisfied]

        if not matches:
            # Nothing in .bazelrc can build this. Bazel would skip it as
            # incompatible, so say why instead of silently dropping it.
            click.secho(
                f"note: skipping {target}, no --config in .bazelrc selects a "
                f"platform satisfying {', '.join(sorted(constraints))}",
                err=True,
                fg="yellow",
            )

            continue

        # Targets may be supported by multiple configs, we only need to add one
        # to the compile commands. Use min for config stability.
        groups[min(matches)].append(target)

    return groups


def generate(generator: str, config: str | None, targets: list[str]) -> list[dict]:
    """Runs the generator over one group of targets and returns its entries."""
    label = config or "host"
    echo(f"{label}: {len(targets)} target(s)")

    with tempfile.NamedTemporaryFile(suffix=".json") as out:
        command = [
            generator,
            # Point entries at the real source files instead of the execroot
            # symlinks, so clangd matches them to the files open in the editor.
            "--resolve",
            "--output",
            out.name,
        ]

        if config:
            command.append(f"--config={config}")

        command += targets

        done = subprocess.run(command, cwd=WORKSPACE, check=False)
        if done.returncode != 0:
            raise click.ClickException(f"generating compile commands for {label} failed")
        return json.loads(pathlib.Path(out.name).read_text())


def anchor_to_workspace(entries: list[dict]) -> None:
    """Rewrites each entry's `directory` from Bazel's execroot to the workspace."""
    output_base = pathlib.Path(bazel("info", "output_base").strip())
    link = WORKSPACE / WORKSPACE_EXTERNAL_CACHE
    target = output_base / WORKSPACE_EXTERNAL_CACHE
    if not link.is_symlink() or link.readlink() != target:
        link.unlink(missing_ok=True)
        link.symlink_to(target, target_is_directory=True)

    for entry in entries:
        entry["directory"] = str(WORKSPACE)


def key(entry: dict) -> str:
    """Gets the object file a compile command produces."""
    return entry.get("output") or entry["file"]


@click.command(context_settings={"help_option_names": ["-h", "--help"]})
@click.argument("targets", nargs=-1)
@click.option(
    "--clean",
    is_flag=True,
    help="Discard the existing compile_commands.json instead of merging into it.",
)
def main(targets: tuple[str, ...], clean: bool) -> None:
    """Generate the compile_commands.json for clangd."""
    if not WORKSPACE:
        raise click.ClickException("BUILD_WORKSPACE_DIRECTORY is unset; run this with `bazel run`")
    patterns = list(targets) or ["//..."]

    generator = runfiles.Create().Rlocation(os.environ["BAZEL_COMPILE_COMMANDS"])

    groups = group_by_config(declared_constraints(patterns), platform_configs())
    if not groups:
        raise click.ClickException(f"no targets matched {' '.join(patterns)}")

    entries = {}
    database = WORKSPACE / "compile_commands.json"
    if not clean and database.exists():
        entries = {key(e): e for e in json.loads(database.read_text())}
    kept = len(entries)

    for config in sorted(groups, key=lambda name: (name is not None, name or "")):
        produced = generate(generator, config, groups[config])
        anchor_to_workspace(produced)
        for entry in produced:
            entries[key(entry)] = entry

    database.write_text(json.dumps(list(entries.values()), indent=2))
    echo(f"{database.name}: {len(entries)} entries ({len(entries) - kept:+d} from this run)")


if __name__ == "__main__":
    # Named for how it's invoked, so --help shows a command that can be pasted
    # rather than the path of the script inside the runfiles tree.
    main(prog_name="bazel run //:generate_compile_commands --")

"""Generates the compile_commands.json that clangd reads.

uv run generate_compile_commands //path/to:target
uv run generate_compile_commands //... --clean

Entries accumulate into the json by default so generating sub-targets does
not override existing targets. Use `--clean` to override the existing json.
"""

import json
import os
import pathlib
import subprocess
import tempfile

import click
from python.runfiles import runfiles  # pyright: ignore[reportMissingImports]

# Get the workspace directoy provided by `bazel run`.
WORKSPACE = pathlib.Path(os.environ.get("BUILD_WORKSPACE_DIRECTORY", ""))

# Symlinked directory that holds external bazel builds for path stability.
WORKSPACE_EXTERNAL_CACHE = pathlib.Path("external")


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


def generate(generator: str, patterns: list[str]) -> list[dict]:
    """Runs the generator over the target patterns and returns its entries."""
    joined = " ".join(patterns)
    echo(f"generating compile commands for {joined}")

    with tempfile.NamedTemporaryFile(suffix=".json") as out:
        command = [
            generator,
            # Point entries at the real source files instead of the execroot
            # symlinks, so clangd matches them to the files open in the editor.
            "--resolve",
            "--output",
            out.name,
            *patterns,
        ]

        done = subprocess.run(command, cwd=WORKSPACE, check=False)
        if done.returncode != 0:
            raise click.ClickException(f"generating compile commands for {joined} failed")
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

    run = runfiles.Create()
    generator = run.Rlocation(os.environ["BAZEL_COMPILE_COMMANDS"]) if run else None
    if generator is None:
        raise click.ClickException("BAZEL_COMPILE_COMMANDS is missing from the runfiles")

    entries = {}
    database = WORKSPACE / "compile_commands.json"
    if not clean and database.exists():
        entries = {key(e): e for e in json.loads(database.read_text())}
    kept = len(entries)

    produced = generate(generator, patterns)
    if not produced:
        raise click.ClickException(f"no C++ targets matched {' '.join(patterns)}")

    anchor_to_workspace(produced)
    for entry in produced:
        entries[key(entry)] = entry

    database.write_text(json.dumps(list(entries.values()), indent=2))
    echo(f"{database.name}: {len(entries)} entries ({len(entries) - kept:+d} from this run)")


if __name__ == "__main__":
    # Named for how it's invoked, so --help shows a command that can be pasted
    # rather than the path of the script inside the runfiles tree.
    main(prog_name="bazel run //:generate_compile_commands --")

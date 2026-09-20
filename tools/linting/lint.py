import dataclasses
import os
import pathlib
import subprocess
from collections import abc

import click
import gitignore_parser
from python.runfiles import runfiles  # pyright: ignore[reportMissingImports]

# Get the workspace directory provided by `bazel run`.
WORKSPACE = pathlib.Path(os.environ.get("BUILD_WORKSPACE_DIRECTORY", ""))


# File extensions for C/C++ code that we want to reject from the repo. And a mapping to the correct
# extension.
REJECTED_CPP_FILE_EXTENSIONS = {"hpp": "h", "cpp": "cc"}

# File extensions for C/C++ code we want to lint.
ACCEPTED_CPP_FILE_EXTENSIONS = ("cc", "h", "c")


def _tool(variable: str) -> str:
    """Gets the hermetic bazel tool from runfiles."""
    run = runfiles.Create()
    path = run.Rlocation(os.environ[variable]) if run else None
    if path is None:
        raise click.ClickException(f"{variable} is missing from the runfiles")
    return path


def _python_commands(fix: bool) -> list[list[str]]:
    """Builds command lists python linting"""
    ruff = _tool("RUFF")
    return [
        [ruff, "check", *([] if fix else ["--no-fix"]), str(WORKSPACE)],
        [ruff, "format", *([] if fix else ["--check"]), str(WORKSPACE)],
    ]


def python_lint(fix: bool) -> bool:
    """Lints python files. Returns True if lint was succesfull (no errors / all
    files fixed)."""
    failed = False
    for cmd in _python_commands(fix):
        click.echo(f"$ {' '.join(cmd)}")
        if subprocess.run(cmd, cwd=WORKSPACE, check=False).returncode != 0:
            failed = True
    return not failed


def get_extension(path: pathlib.Path) -> str | None:
    "Gets the extension of the file at the path, or none, does not inclue '.'"
    if "." in path.name:
        return str(path).split(".")[-1]
    return None


def get_repo_files(extensions: list[str]) -> list[pathlib.Path]:
    """Gets all files checked into the repo, .gitignore aware.

    extensions is a list of file extensions to filter for, an empty list is no
    filter.
    """

    @dataclasses.dataclass
    class Dir:
        path: pathlib.Path
        ignores: list[abc.Callable[[pathlib.Path], bool]]

    dirs: list[Dir] = [
        Dir(
            path=WORKSPACE,
            ignores=[],
        )
    ]

    out: list[pathlib.Path] = []
    while dirs:
        curr: Dir = dirs.pop(0)
        maybe_gitignore = curr.path / ".gitignore"
        if maybe_gitignore.exists():
            curr.ignores.append(gitignore_parser.parse_gitignore(maybe_gitignore))

        for f in curr.path.iterdir():
            if any(ignore(f) for ignore in curr.ignores):
                continue
            if f.is_file():
                if not extensions or get_extension(f) in extensions:
                    out.append(f)
            else:
                dirs.append(Dir(path=f, ignores=curr.ignores.copy()))
    return out


def _cpp_commands(fix: bool) -> list[list[str]]:
    """Builds command lists python linting"""
    clang_format = _tool("CLANG_FORMAT")
    return [
        [
            clang_format,
            "-i",
            "--style=file",
            *(() if fix else ("--dry-run",)),
            "--color",
            str(f),
        ]
        for f in get_repo_files(ACCEPTED_CPP_FILE_EXTENSIONS)
    ]


def cpp_lint(fix: bool) -> bool:
    """Lints c++ files. Returns True if lint was succesfull (no errors / all
    files fixed)."""

    failed = False

    # Repalce / log invalid C++ extensions.
    if files := get_repo_files(REJECTED_CPP_FILE_EXTENSIONS.keys()):
        click.echo("Found invalid C++ extensions.")
        for f in files:
            if fix:
                new_ext = REJECTED_CPP_FILE_EXTENSIONS[get_extension(f)]
                split = str(f).split(".")
                split[-1] = new_ext
                new_name = ".".join(split)
                f.rename(new_name)
                failed = True

            else:
                click.echo(f"\t{f!s}")

        return True

    for cmd in _cpp_commands(fix):
        result = subprocess.run(
            cmd,
            cwd=WORKSPACE,
            check=True,
            capture_output=True,
            text=True,
        )

        if result.stderr:
            print(result.stderr)
            failed = True
    return not failed


@click.command(context_settings={"help_option_names": ["-h", "--help"]})
@click.option("--check", is_flag=True, help="Only checks, does not auto-fix.")
def main(check: bool) -> None:
    """Lint the workspace's python sources."""
    if not WORKSPACE:
        raise click.ClickException("BUILD_WORKSPACE_DIRECTORY is unset; run this with `bazel run`")

    if not python_lint(not check) or not cpp_lint(not check):
        raise click.ClickException("Linting Contains Errors.")
    click.echo("Linting succesfulll.")


if __name__ == "__main__":
    main(prog_name="bazel run //tools/linting:lint --")

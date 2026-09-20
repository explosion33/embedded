import os
import pathlib
import subprocess

import click
from python.runfiles import runfiles  # pyright: ignore[reportMissingImports]

# Get the workspace directory provided by `bazel run`.
WORKSPACE = pathlib.Path(os.environ.get("BUILD_WORKSPACE_DIRECTORY", ""))


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


@click.command(context_settings={"help_option_names": ["-h", "--help"]})
@click.option("--check", is_flag=True, help="Only checks, does not auto-fix.")
def main(check: bool) -> None:
    """Lint the workspace's python sources."""
    if not WORKSPACE:
        raise click.ClickException(
            "BUILD_WORKSPACE_DIRECTORY is unset; run this with `bazel run`"
        )

    if not python_lint(not check):
        raise click.ClickException("Linting Contains Errors.")
    click.echo("Linting succesfulll.")


if __name__ == "__main__":
    main(prog_name="bazel run //tools/linting:lint --")

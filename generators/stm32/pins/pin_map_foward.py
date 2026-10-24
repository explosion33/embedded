import pathlib

import click
import jinja2


@click.command()
@click.option("--template", required=True, type=pathlib.Path)
@click.option("--output", required=True, type=pathlib.Path)
@click.option("--namespace", required=True, type=str)
def main(
    template: pathlib.Path,
    output: pathlib.Path,
    namespace: str,
):
    env = jinja2.Environment(
        loader=jinja2.FileSystemLoader(template.parent), trim_blocks=True, lstrip_blocks=True
    )
    j2_template = env.get_template(template.name)

    content = j2_template.render(
        namespace=namespace,
    )
    output.write_text(content)


if __name__ == "__main__":
    main()

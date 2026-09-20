Start of the embedded development monorepo.

# Development
## C++ Linting
C/C++ completion, navigation, and linting comes from `clangd`, which reads the `compile_commands.json` at the repo root. This can be generated with
```
bazel run //:generate_compile_commands -- //path/to/targets/...
bazel run //:generate_compile_commands -- //... --clean
```

In VS Code, install the
[clangd extension](https://marketplace.visualstudio.com/items?itemName=llvm-vs-code-extensions.vscode-clangd).

## Python dependencies
Run To update the uv lock after updating python dependencies.
```
bazel run //:uv.update
```
TODO: Move this into a generic generate script, this script should run in CI and check for differences before passing.

## Firmware

cc_libraries can be cross compiled into firmware binaries (.elf and .bin). Each
`cc_fw_app` names the `platform` it is built for and cross compiles itself to
that platform.

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

Compiled firmware can be flashed to a connected board via a debug probe (i.e. STLINK / NUCLEO) using the following tool:
```
bazel run //fw/tools:flash -- /absolute/path/to/app.bin --target=<target>
```

This will auto discover and select the first compatible device found that supports `<target>`. The target is a family prefix search, i.e. given the following device the following targets will match on it:
```
Physical Target   : "STM32F446RETX"
Matching <target>s: "STM", "STM32", "STM32F4", "STM32F446", ..., etc.
```
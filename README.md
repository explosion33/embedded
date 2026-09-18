Start of the embedded development monorepo.

# Development
## Python dependencies
Run To update the uv lock after updating python dependencies.
```
bazel run //:uv.update
```
TODO: Move this into a generic generate script, this script should run in CI and check for differences before passing.

## Firmware

cc_libraries can be cross compiled into firmware binaries (.elf and .bin). Each supported device has a `--config` in `.bazelrc`. Example usage:
```
bazel build --config=f446 //path/to:app
```
That produces `app.elf`, `app.bin` and `app.elf.map` in
`bazel-bin/` for the STM32F446XX, these binaries can then be flashed at address `0x08000000`.

Each `cc_fw_app` declares a list of its compatible targets, which can only be built with the matching `--config` flag. Build is skipped for catch all bazel invocations.
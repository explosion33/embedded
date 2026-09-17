Start of the embedded development monorepo.

# Development
## Python dependencies
Run To update the uv lock after updating python dependencies.
```
bazel run //:uv.update
```
TODO: Move this into a generic generate script, this script should run in CI and check for differences before passing.
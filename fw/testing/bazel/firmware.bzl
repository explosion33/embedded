load("@rules_python//python:py_test.bzl", "py_test")
load("//fw/platform/bazel:firmware.bzl", "cc_fw_app")

# TODO: `target` could probably be derived from `board` if we do:
#   * Convert flash script to scan board names rather than target
#     names
#   * Define a list of compatible targets for a given board.
def cc_fw_test(
        name,
        board,
        target,
        srcs = [],
        deps = [],
        **kwargs):
    """Compiles, flashes, and records responses for a test. The test must use the provided test harness.

    Args:
      name: Name of the firmware image.
      board: The board package to build for, defined in //fw/boards/. The board
        package must provide a `:platform` to compile for, startup code, and
        linker script.
      target: The target this test should be flashed to.
      srcs: C/C++/assembly sources.
      **kwargs: Args passed through to the underyling binary.
      deps: cc_library/binary dependencies for this app.
    """
    image = name + "_image"
    cc_fw_app(name = image, board = board, srcs = srcs, deps = deps)

    native.filegroup(
        name = name + "_bin",
        srcs = [":" + image],
        output_group = "bin",
    )

    py_test(
        name = name,
        srcs = ["//fw/testing:run_test.py"],
        main = "//fw/testing:run_test.py",
        data = [":" + name + "_bin"],
        args = [
            "--binary_rlocation",
            "$(rlocationpath  :%s_bin)" % name,
            "--target",
            target,
        ],
        deps = [
            "//fw/tools:flash",
            "@pypi//click",
            "@pypi//pyserial",
            "@rules_python//python/runfiles",
        ],
        # Only one test can own the board and serial port at a time.
        tags = ["exclusive", "manual"],
        **kwargs
    )

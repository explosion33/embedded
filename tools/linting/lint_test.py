import pathlib
import tempfile
import unittest
from unittest import mock

from tools.linting import lint


class Test(unittest.TestCase):
    def test_get_extension(self):
        self.assertEqual(lint.get_extension(pathlib.Path("test.cc")), "cc")
        self.assertEqual(lint.get_extension(pathlib.Path("test.h")), "h")
        self.assertEqual(lint.get_extension(pathlib.Path("test.cc.h")), "h")
        self.assertEqual(lint.get_extension(pathlib.Path(".git/test.h")), "h")
        self.assertEqual(lint.get_extension(pathlib.Path(".git/test")), None)
        self.assertEqual(lint.get_extension(pathlib.Path("test")), None)
        self.assertEqual(lint.get_extension(pathlib.Path(".test")), "test")

    def test_get_repo_files(self):
        with (
            tempfile.TemporaryDirectory() as dir,
            mock.patch.object(lint, "WORKSPACE", pathlib.Path(dir)),
        ):
            path = pathlib.Path(dir)

            expected_files = []

            def make(path: pathlib.Path, ignored: bool = False, text: str = "") -> None:
                path.write_text(text)
                if not ignored:
                    expected_files.append(path)

            make(path / ".gitignore", False, "ignored_file\n*/ignored_file\n/ignored_dir/\n")

            (path / "ignored_dir").mkdir()
            (path / "dir").mkdir()

            make(path / "ignored_file", True)
            make(path / "file_0")
            make(path / "file_1")
            make(path / "file_2")

            make(path / "ignored_dir/file_0", True)
            make(path / "ignored_dir/file_1", True)
            make(path / "ignored_dir/file_2", True)

            make(path / "dir/.gitignore", False, "/ignored_file2\n")
            make(path / "dir/file_0")
            make(path / "dir/file_1")
            make(path / "dir/file_2")
            make(path / "dir/ignored_file", True)
            make(path / "dir/ignored_file2", True)

            self.assertCountEqual(lint.get_repo_files([]), expected_files)

    def test_get_repo_files_extensions(self):
        with (
            tempfile.TemporaryDirectory() as dir,
            mock.patch.object(lint, "WORKSPACE", pathlib.Path(dir)),
        ):
            root = pathlib.Path(dir)

            expected_files = []

            def make(path: pathlib.Path) -> None:
                full = root / path
                full.touch()
                if str(path).endswith(".cc") or str(path).endswith(".h"):
                    expected_files.append(full)

            make("test.cc")
            make("test.h")
            make("test1.cc")
            make("test1.h")
            make("test2.cc")
            make("test2.h")

            make("ahh")
            make("ahh.ccc")
            make("ahh.hh")
            make("ahh.cc.h.test")

            self.assertCountEqual(lint.get_repo_files(["cc", "h"]), expected_files)


if __name__ == "__main__":
    unittest.main()

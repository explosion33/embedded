# A simple test to validate python bazel builds.
# TODO: Remove once out first python test exists.

import unittest


class Test(unittest.TestCase):
    def test_true(self):
        self.assertTrue(True)


if __name__ == "__main__":
    unittest.main()

#include "greatest.h"
#include "kioku/log.h"

TEST TestSourcePath(void)
{
  const char *path = NULL;

  /* Test bad input */
  path = srsLog_GetSourcePath(NULL);
  ASSERT_EQ(path, NULL);

  /* Test this file */
  path = srsLog_GetSourcePath(__FILE__);
  srsLOG_PRINT("Source Path: %s", path);
  ASSERT(strcmp(path, "test/log.c") == 0 || strcmp(path, "test\\log.c") == 0);

  /**
   * This is a good example of how testing a very simple function can explode.
   * There's a lot of code duplication here with simple variations.
   * - \\ or / separators
   * - drive letter or no
   * - leading separator or no
   * - starts with src/something/ versus src/kioku/ (TODO not tested for yet)
   * - starts with include/something/ versus include/kioku/ (TODO not tested for yet)
   * - mixed \\ or / (TODO not accounted for in implementation or tested for yet)
   */

  /* Prefix with test tests */

  /* Test '/' path with drive letter */
  path = srsLog_GetSourcePath("E:/test/path/stuff.c");
  ASSERT_STR_EQ("test/path/stuff.c", path);
  /* Test '/' path without drive letter */
  path = srsLog_GetSourcePath("/test/path/stuff.c");
  ASSERT_STR_EQ("test/path/stuff.c", path);
  /* Test '/' path without drive letter or leading slash */
  path = srsLog_GetSourcePath("test/path/stuff.c");
  ASSERT_STR_EQ("test/path/stuff.c", path);

  /* Test '\\' path with drive letter */
  path = srsLog_GetSourcePath("E:\\test\\path\\stuff.c");
  ASSERT_STR_EQ("test\\path\\stuff.c", path);
  /* Test '\\' path without drive letter */
  path = srsLog_GetSourcePath("\\test\\path\\stuff.c");
  ASSERT_STR_EQ("test\\path\\stuff.c", path);
  /* Test '\\' path without drive letter or leading slash */
  path = srsLog_GetSourcePath("test\\path\\stuff.c");
  ASSERT_STR_EQ("test\\path\\stuff.c", path);

  /* Prefix with src tests */

  /* Test '/' path with drive letter */
  path = srsLog_GetSourcePath("E:/src/path/stuff.c");
  ASSERT_STR_EQ("src/path/stuff.c", path);
  /* Test '/' path without drive letter */
  path = srsLog_GetSourcePath("/src/path/stuff.c");
  ASSERT_STR_EQ("src/path/stuff.c", path);
  /* Test '/' path without drive letter or leading slash */
  path = srsLog_GetSourcePath("src/path/stuff.c");
  ASSERT_STR_EQ("src/path/stuff.c", path);

  /* Test '\\' path with drive letter */
  path = srsLog_GetSourcePath("E:\\src\\path\\stuff.c");
  ASSERT_STR_EQ("src\\path\\stuff.c", path);
  /* Test '\\' path without drive letter */
  path = srsLog_GetSourcePath("\\src\\path\\stuff.c");
  ASSERT_STR_EQ("src\\path\\stuff.c", path);
  /* Test '\\' path without drive letter or leading slash */
  path = srsLog_GetSourcePath("src\\path\\stuff.c");
  ASSERT_STR_EQ("src\\path\\stuff.c", path);

  /* Prefix with include tests */

  /* Test '/' path with drive letter */
  path = srsLog_GetSourcePath("E:/include/path/stuff.c");
  ASSERT_STR_EQ("include/path/stuff.c", path);
  /* Test '/' path without drive letter */
  path = srsLog_GetSourcePath("/include/path/stuff.c");
  ASSERT_STR_EQ("include/path/stuff.c", path);
  /* Test '/' path without drive letter or leading slash */
  path = srsLog_GetSourcePath("include/path/stuff.c");
  ASSERT_STR_EQ("include/path/stuff.c", path);

  /* Test '\\' path with drive letter */
  path = srsLog_GetSourcePath("E:\\include\\path\\stuff.c");
  ASSERT_STR_EQ("include\\path\\stuff.c", path);
  /* Test '\\' path without drive letter */
  path = srsLog_GetSourcePath("\\include\\path\\stuff.c");
  ASSERT_STR_EQ("include\\path\\stuff.c", path);
  /* Test '\\' path without drive letter or leading slash */
  path = srsLog_GetSourcePath("include\\path\\stuff.c");
  ASSERT_STR_EQ("include\\path\\stuff.c", path);

  PASS();
}

/* Suites can group multiple tests with common setup. */
SUITE(the_suite) {
  RUN_TEST(TestSourcePath);
}

/* Add definitions that need to be in the test runner's main file. */
GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
  GREATEST_MAIN_BEGIN();      /* command-line options, initialization. */

  /* Individual tests can be run directly. */
  /* RUN_TEST(x_should_equal_1); */

  /* Tests can also be gathered into test suites. */
  RUN_SUITE(the_suite);

  GREATEST_MAIN_END();        /* display results */
}

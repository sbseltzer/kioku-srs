#include "greatest.h"
#include "kioku/string.h"
#include "kioku/log.h"

/* char *, bool, int32 */
#define test_number(str, isnum, num)                        \
  do {                                                      \
    int32_t output_number = 0;                              \
    ASSERT_EQ(isnum, srsString_ToU32(str, NULL));           \
    ASSERT_EQ(isnum, srsString_ToU32(str, &output_number)); \
    if (isnum)                                              \
    {                                                       \
      ASSERT_EQ(num, output_number);                        \
    }                                                       \
  } while (0)

/* A test runs various assertions, then calls PASS(), FAIL(), or SKIP(). */
TEST TestStringToU32(void)
{
  int32_t number = 0;

  ASSERT_FALSE(srsString_ToU32(NULL, NULL));
  ASSERT_FALSE(srsString_ToU32(NULL, &number));

  test_number("0",  true, 0);
  test_number("0abc",  true, 0);
  test_number("000", true, 0);
  test_number("000abc", true, 0);

  test_number("1",  true, 1);
  test_number("1abc",  true, 1);
  test_number("12345", true, 12345);
  test_number("12345abc", true, 12345);

  test_number("-1", true, -1);
  test_number("-1abc", true, -1);
  test_number("-12345", true, -12345);
  test_number("-12345abc", true, -12345);

  test_number("1111111111111111111111111111111111111111111", false, 0);
  test_number("-1111111111111111111111111111111111111111111", false, 0);

  test_number("abc", false, 0);
  test_number("-abc", false, 0);
  test_number("", false, 0);

  PASS();
}

TEST TestSourcePath(void)
{
  const char *path = NULL;

  /* Test bad input */
  path = srsString_GetSourcePath(NULL);
  ASSERT_EQ(path, NULL);

  /* Test this file */
  path = srsString_GetSourcePath(__FILE__);
  srsLOG_NOTIFY("Source Path: %s", path);
  ASSERT(strcmp(path, "test/string.c") == 0 || strcmp(path, "test\\string.c") == 0);

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
  path = srsString_GetSourcePath("E:/test/path/stuff.c");
  ASSERT_STR_EQ("test/path/stuff.c", path);
  /* Test '/' path without drive letter */
  path = srsString_GetSourcePath("/test/path/stuff.c");
  ASSERT_STR_EQ("test/path/stuff.c", path);
  /* Test '/' path without drive letter or leading slash */
  path = srsString_GetSourcePath("test/path/stuff.c");
  ASSERT_STR_EQ("test/path/stuff.c", path);

  /* Test '\\' path with drive letter */
  path = srsString_GetSourcePath("E:\\test\\path\\stuff.c");
  ASSERT_STR_EQ("test\\path\\stuff.c", path);
  /* Test '\\' path without drive letter */
  path = srsString_GetSourcePath("\\test\\path\\stuff.c");
  ASSERT_STR_EQ("test\\path\\stuff.c", path);
  /* Test '\\' path without drive letter or leading slash */
  path = srsString_GetSourcePath("test\\path\\stuff.c");
  ASSERT_STR_EQ("test\\path\\stuff.c", path);

  /* Prefix with src tests */

  /* Test '/' path with drive letter */
  path = srsString_GetSourcePath("E:/src/path/stuff.c");
  ASSERT_STR_EQ("src/path/stuff.c", path);
  /* Test '/' path without drive letter */
  path = srsString_GetSourcePath("/src/path/stuff.c");
  ASSERT_STR_EQ("src/path/stuff.c", path);
  /* Test '/' path without drive letter or leading slash */
  path = srsString_GetSourcePath("src/path/stuff.c");
  ASSERT_STR_EQ("src/path/stuff.c", path);

  /* Test '\\' path with drive letter */
  path = srsString_GetSourcePath("E:\\src\\path\\stuff.c");
  ASSERT_STR_EQ("src\\path\\stuff.c", path);
  /* Test '\\' path without drive letter */
  path = srsString_GetSourcePath("\\src\\path\\stuff.c");
  ASSERT_STR_EQ("src\\path\\stuff.c", path);
  /* Test '\\' path without drive letter or leading slash */
  path = srsString_GetSourcePath("src\\path\\stuff.c");
  ASSERT_STR_EQ("src\\path\\stuff.c", path);

  /* Prefix with include tests */

  /* Test '/' path with drive letter */
  path = srsString_GetSourcePath("E:/include/path/stuff.c");
  ASSERT_STR_EQ("include/path/stuff.c", path);
  /* Test '/' path without drive letter */
  path = srsString_GetSourcePath("/include/path/stuff.c");
  ASSERT_STR_EQ("include/path/stuff.c", path);
  /* Test '/' path without drive letter or leading slash */
  path = srsString_GetSourcePath("include/path/stuff.c");
  ASSERT_STR_EQ("include/path/stuff.c", path);

  /* Test '\\' path with drive letter */
  path = srsString_GetSourcePath("E:\\include\\path\\stuff.c");
  ASSERT_STR_EQ("include\\path\\stuff.c", path);
  /* Test '\\' path without drive letter */
  path = srsString_GetSourcePath("\\include\\path\\stuff.c");
  ASSERT_STR_EQ("include\\path\\stuff.c", path);
  /* Test '\\' path without drive letter or leading slash */
  path = srsString_GetSourcePath("include\\path\\stuff.c");
  ASSERT_STR_EQ("include\\path\\stuff.c", path);

  PASS();
}

/* Suites can group multiple tests with common setup. */
SUITE(the_suite) {
  RUN_TEST(TestStringToU32);
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

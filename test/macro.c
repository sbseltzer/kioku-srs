#include "greatest.h"
#include "kioku/decl.h"
#include "kioku/log.h"

TEST TestFunctionName(void)
{
  ASSERT_STR_EQ(srsFUNCTION_NAME, "Example");
  PASS();
}
TEST TestStrBool(void)
{
  ASSERT_STR_EQ(srsLOG_STRBOOL(true), "true");
  ASSERT_STR_EQ(srsLOG_STRBOOL(false), "false");
  PASS();
}

/* Suites can group multiple tests with common setup. */
SUITE(the_suite) {
  RUN_TEST(TestExample);
  RUN_TEST(TestStrBool);
}

/* Add definitions that need to be in the test runner's main file. */
GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
  GREATEST_MAIN_BEGIN();      /* command-line options, initialization. */

  /* Tests can also be gathered into test suites. */
  RUN_SUITE(the_suite);

  GREATEST_MAIN_END();        /* display results */
}

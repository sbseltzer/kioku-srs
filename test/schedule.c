
#include "greatest.h"
#include "kioku/schedule.h"

/* A test runs various assertions, then calls PASS(), FAIL(), or SKIP(). */
TEST TimeConvertsToAndFromString(void)
{
  srsTIME_STRING timestr = {0};
  srsTIME time = {0};
  ASSERT_FALSE(srsTime_FromString(NULL, &time));
  ASSERT_FALSE(srsTime_FromString(timestr, NULL));
  ASSERT_FALSE(srsTime_FromString(NULL, NULL));
  const char *in = NULL;
  in = "2017-10-31 23:59";
  ASSERT(srsTime_FromString(in, &time));
  ASSERT_EQ(2017, time.year);
  ASSERT_EQ(10, time.month);
  ASSERT_EQ(31, time.day);
  ASSERT_EQ(23, time.hour);
  ASSERT_EQ(59, time.minute);
  ASSERT(srsTime_ToString(time, timestr));
  ASSERT_STR_EQ(in, timestr);
  /** @todo Test out of range stuff */
  /** @todo Test format mismatches */
  PASS();
}


/* Suites can group multiple tests with common setup. */
SUITE(the_suite) {
  RUN_TEST(TimeConvertsToAndFromString);
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

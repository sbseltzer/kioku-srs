#include "greatest.h"
#include "kioku/datastructure.h"

/* A test runs various assertions, then calls PASS(), FAIL(), or SKIP(). */
TEST TestMemStack_Init(void)
{
  srsMEMSTACK stack = {0};
  uint8_t value = 0;
  /* Test invalid input */
  ASSERT_FALSE(srsMemStack_Init(NULL, sizeof(value), -1));
  ASSERT_FALSE(srsMemStack_Init(&stack, 0, -1));

  /* Test that initial_capacity of -1 results in a minimum capacity being chosen */
  ASSERT(srsMemStack_Init(&stack, sizeof(value), -1));
  ASSERT_EQ(srsMEMSTACK_MINIMUM_CAPACITY, stack.capacity);
  ASSERT(srsMemStack_FreeContents(&stack));

  /* Test that initial_capacity of 0 results in a minimum capacity being chosen */
  ASSERT(srsMemStack_Init(&stack, sizeof(value), 0));
  ASSERT_EQ(srsMEMSTACK_MINIMUM_CAPACITY, stack.capacity);
  ASSERT(srsMemStack_FreeContents(&stack));

  /* Test all members of stack with a few elements */
  size_t capacity = 3;
  ASSERT(srsMemStack_Init(&stack, sizeof(value), capacity));
  ASSERT_EQ(0, stack.count);
  ASSERT_EQ(NULL, stack.top);
  ASSERT_NEQ(NULL, stack.memory);
  ASSERT_EQ(capacity, stack.capacity);
  ASSERT_EQ(sizeof(value), stack.element_size);

  /* Test first push on stack of 1 */
  ASSERT(srsMemStack_Init(&stack, 1, 1));
  ASSERT(srsMemStack_Push(&stack, &value));

  PASS();
}


/* Suites can group multiple tests with common setup. */
SUITE(the_suite) {
  RUN_TEST(TestMemStack_Init);
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

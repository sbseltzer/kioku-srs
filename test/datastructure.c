#include "greatest.h"
#include "kioku/log.h"
#include "kioku/datastructure.h"

/* A test runs various assertions, then calls PASS(), FAIL(), or SKIP(). */
TEST TestMemStack_InitAndFree(void)
{
  srsMEMSTACK stack = {0};
  uint8_t value = 0;
  srsLOG_NOTIFY("Testing bad input for memstack");
  /* Test invalid input */
  ASSERT_FALSE(srsMemStack_Init(NULL, sizeof(value), -1));
  ASSERT_FALSE(srsMemStack_Init(&stack, 0, -1));
  ASSERT_FALSE(srsMemStack_FreeContents(NULL));
  ASSERT_FALSE(srsMemStack_FreeContents(&stack));

  srsLOG_NOTIFY("Testing input defaulting for memstack");

  /* Test that initial_capacity of -1 results in a minimum capacity being chosen */
  ASSERT(srsMemStack_Init(&stack, sizeof(value), -1));
  ASSERT_EQ(srsMEMSTACK_MINIMUM_CAPACITY, stack.capacity);
  ASSERT(srsMemStack_FreeContents(&stack));

  /* Test that initial_capacity of 0 results in a minimum capacity being chosen */
  ASSERT(srsMemStack_Init(&stack, sizeof(value), 0));
  ASSERT_EQ(srsMEMSTACK_MINIMUM_CAPACITY, stack.capacity);
  ASSERT(srsMemStack_FreeContents(&stack));

  srsLOG_NOTIFY("Testing results of free on memstack contents");
  /* Test all members of stack with a few elements */
  int32_t capacity = 3;
  ASSERT(srsMemStack_Init(&stack, sizeof(value), capacity));
  ASSERT_EQ(0, stack.count);
  ASSERT_EQ(NULL, stack.top);
  ASSERT(NULL != stack.memory);
  ASSERT_EQ(capacity, stack.capacity);
  ASSERT_EQ(sizeof(value), stack.element_size);
  ASSERT(srsMemStack_FreeContents(&stack));
  srsLOG_NOTIFY("Testing results of free on memstack contents");
  srsMEMSTACK zeroed = {0};
  ASSERT_EQ(0, memcmp(&stack, &zeroed, sizeof(zeroed)));
  PASS();
}

TEST TestMemStack_Push1Pop1(void)
{
  srsMEMSTACK stack = {0};
  uint8_t value_in = 200;
  uint8_t value_out = 0;
  /* Test bad input */
  ASSERT_FALSE(srsMemStack_Push(NULL, NULL));
  ASSERT_FALSE(srsMemStack_Push(NULL, &value_in));
  ASSERT_FALSE(srsMemStack_Push(&stack, NULL));
  ASSERT_FALSE(srsMemStack_Pop(NULL, NULL));
  ASSERT_FALSE(srsMemStack_Pop(&stack, &value_out));
  ASSERT_FALSE(srsMemStack_Pop(&stack, NULL));

  /* Test first push on stack of 1 */
  ASSERT(srsMemStack_Init(&stack, sizeof(value_in), 1));
  srsMEMSTACK original_state = stack;

  srsLOG_NOTIFY("Testing push on smallest possible stack capacity");
  srsMEMSTACK_PRINT(stack);
  ASSERT(srsMemStack_Push(&stack, &value_in));
  srsMEMSTACK_PRINT(stack);
  ASSERT_EQ_FMT(stack.memory, stack.top, "%p");
  ASSERT_EQ_FMT(value_in, *((uint8_t *)stack.top), "%u");

  srsLOG_NOTIFY("Testing pop from smallest stack capacity");
  ASSERT(srsMemStack_Pop(&stack, &value_out));
  srsMEMSTACK_PRINT(stack);
  ASSERT_EQ_FMT(value_in, value_out, "%u");
  ASSERT_EQ_FMT(original_state.top, stack.top, "%p");
  ASSERT_EQ_FMT(original_state.count, stack.count, "%zu");
  /* Adding an element doubled the capacity to 2 */
  ASSERT_EQ_FMT(original_state.capacity * 2, stack.capacity, "%zu");

  ASSERT(srsMemStack_FreeContents(&stack));

  PASS();
}

TEST TestMemStack_PopFromEmptyFailsWithNoOutput(void)
{
  srsMEMSTACK stack = {0};
  uint8_t value_in = 200;
  uint8_t value_out = 0;

  ASSERT(srsMemStack_Init(&stack, sizeof(value_in), 1));
  srsMEMSTACK original_state = stack;

  srsLOG_NOTIFY("Testing pop from empty stack");
  ASSERT_EQ(false, srsMemStack_Pop(&stack, &value_out));
  ASSERT_EQ_FMT(0, value_out, "%u");
  ASSERT_EQ_FMT(original_state.top, stack.top, "%p");
  ASSERT_EQ_FMT(original_state.count, stack.count, "%zu");
  ASSERT_EQ_FMT(original_state.capacity, stack.capacity, "%zu");
  ASSERT(srsMemStack_FreeContents(&stack));

  PASS();
}

/* TODO Add test to make sure memory doesn't change when capacity is never changed */

TEST TestMemStack_Push4Pop4WorksAndIncreasesCapacity(void)
{
  srsMEMSTACK stack = {0};
  int32_t values[4] = {12, 25, -27, 0};
  int32_t value_in = 0;
  int32_t value_out = 0;

  ASSERT(srsMemStack_Init(&stack, sizeof(value_in), 1));
  srsMEMSTACK original_state = stack;
  size_t i;
  /* Perform pushes */
  for (i = 1; i <= 4; i++)
  {
    value_in = values[i-1];
    srsMEMSTACK_PRINT(stack);
    /* See if count is what we expect */
    ASSERT_EQ_FMT(i-1, stack.count, "%zu");
    /* Push */
    srsLOG_NOTIFY("Pushing value #%zu (%d)", i, value_in);
    ASSERT(srsMemStack_Push(&stack, &value_in));
    srsMEMSTACK_PRINT(stack);
    /* See if top of stack updated to expected value */
    ASSERT_MEM_EQ(&value_in, stack.top, stack.element_size);
    /* See if count updated */
    ASSERT_EQ_FMT(i, stack.count, "%zu");
    /* See if capacity has increased as expected */
    switch(stack.count)
    {
      case 1:
      {
        ASSERT_EQ_FMT(2, stack.capacity, "%zu");
        break;
      }
      case 2:
      case 3:
      {
        ASSERT_EQ_FMT(4, stack.capacity, "%zu");
        break;
      }
      case 4:
      {
        ASSERT_EQ_FMT(8, stack.capacity, "%zu");
        break;
      }
      default:
        ASSERT(false);
    }
  }
  /* Perform pops (we count backward since for the correct index on the values array) */
  for (i = 4; i >= 1; i--)
  {
    value_in = values[i-1];
    srsMEMSTACK_PRINT(stack);
    /* See if count is what we expect */
    ASSERT_EQ_FMT(i, stack.count, "%zu");
    /* Pop */
    srsLOG_NOTIFY("Popping value #%zu (%d)", i, value_in);
    ASSERT(srsMemStack_Pop(&stack, &value_out));
    srsMEMSTACK_PRINT(stack);
    /* See if we got the right value */
    ASSERT_EQ_FMT(value_in, value_out, "%d");
    /* See if top of stack updated to expected value */
    if (i > 1)
    {
      ASSERT_MEM_EQ(&values[i-2], stack.top, stack.element_size);
    }
    else
    {
      ASSERT_EQ_FMT(NULL, stack.top, "%p");
    }
    /* See if count updated */
    ASSERT_EQ_FMT(i-1, stack.count, "%zu");
    /* See if capacity has decreased as expected */
    /* The capacity decreases by 1/2 every time count is less than 1/4 the capacity */
    switch(stack.count)
    {
      case 3:
      case 2:
      {
        ASSERT_EQ_FMT(8, stack.capacity, "%zu");
        break;
      }
      case 1:
      {
        ASSERT_EQ_FMT(4, stack.capacity, "%zu");
        break;
      }
      case 0:
      {
        ASSERT_EQ_FMT(2, stack.capacity, "%zu");
        break;
      }
      default:
        ASSERT(false);
    }
  }
  ASSERT_EQ_FMT(original_state.top, stack.top, "%p");
  ASSERT_EQ_FMT(original_state.count, stack.count, "%zu");
  /* Capacity increased 3 times and decreased twice */
  ASSERT_EQ_FMT(((original_state.capacity * 2 * 2 * 2) / 2) / 2, stack.capacity, "%zu");
  ASSERT(srsMemStack_FreeContents(&stack));

  PASS();
}

/* Suites can group multiple tests with common setup. */
SUITE(the_suite) {
  RUN_TEST(TestMemStack_InitAndFree);
  RUN_TEST(TestMemStack_Push1Pop1);
  RUN_TEST(TestMemStack_PopFromEmptyFailsWithNoOutput);
  RUN_TEST(TestMemStack_Push4Pop4WorksAndIncreasesCapacity);
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

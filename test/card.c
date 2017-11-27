#include "greatest.h"
#include "kioku/model/card.h"
#include "kioku/model/card.h"

static bool add_card(const char *id, srsTIME added, srsTIME scheduled)
{
  srsDir_Create("cards");
  {
    srsDir_PushCWD("cards");
    srsTIME_STRING timestr = {0};
    srsDir_Create(id);
    {
      srsDir_PushCWD(id);
      srsTime_ToString(added, timestr);
      srsFile_Create("added.txt");
      srsFile_SetContent("added.txt", timestr);
      srsTime_ToString(scheduled, timestr);
      srsFile_Create("scheduled.txt");
      srsFile_SetContent("scheduled.txt", timestr);
      srsFile_Create("front.txt");
      srsFile_SetContent("front.txt", "things");
      srsFile_Create("back.txt");
      srsFile_SetContent("back.txt", "stuff");
      srsDir_PopCWD(NULL);
    }
    srsDir_PopCWD(NULL);
  }
  /* TODO We may want to utilize this for an ASSERT */
  return true;
}
/* A test runs various assertions, then calls PASS(), FAIL(), or SKIP(). */
TEST test_card(void)
{
  srsTIME now = srsTime_Now();
  size_t added_count = 0;
  add_card("id_0", now, now); added_count++;
  add_card("id_1", now, now); added_count++;
  add_card("id_2", now, now); added_count++;
  add_card("id_3", now, now); added_count++;

  size_t count = 0;
  srsCARD *cards = srsCard_GetAll(".", &count);
  ASSERT_EQ(added_count, count);

  ASSERT_STR_EQ(cards[0].id, "id_0");
  ASSERT_STR_EQ(cards[1].id, "id_1");
  ASSERT_STR_EQ(cards[2].id, "id_2");
  ASSERT_STR_EQ(cards[3].id, "id_3");

  size_t i = 0;
  for (i = 0; i < count; i++)
  {
    srsDir_PushCWD(cards[i].path);
    ASSERT(srsFile_Exists("added.txt"));
    ASSERT(srsFile_Exists("scheduled.txt"));
    ASSERT_MEM_EQ(&now, &cards[i].when_added, sizeof(now));
    ASSERT_MEM_EQ(&now, &cards[i].when_next_scheduled, sizeof(now));
  }
  /** TODO Test srsCard_GetContent */
  /** TODO Test the content of added/scheduled.txt against now */

  srsRESULT result = srsCard_FreeArray(cards, count);
  ASSERT_EQ(srsOK, result);

  PASS();
}
/* Suites can group multiple tests with common setup. */
SUITE(the_suite) {
  RUN_TEST(test_card);
}

/* Add definitions that need to be in the test runner's main file. */
GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
  GREATEST_MAIN_BEGIN();      /* command-line options, initialization. */

  /* Tests can also be gathered into test suites. */
  RUN_SUITE(the_suite);

  GREATEST_MAIN_END();        /* display results */
}

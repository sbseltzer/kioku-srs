#include "greatest.h"
#include "kioku/model/card.h"
#include "kioku/filesystem.h"
#include "kioku/log.h"
#include "kioku/error.h"
#include <stdlib.h>

static bool add_card(const char *id, srsTIME added, srsTIME scheduled)
{
  srsLOG_PRINT("Creating cards folder");
  srsDir_Create("cards");
  {
    srsDir_PushCWD("cards");
    srsTIME_STRING timestr = {0};
    srsLOG_PRINT("Creating card with ID %s", id);
    srsDir_Create(id);
    {
      srsDir_PushCWD(id);

      srsTime_ToString(added, timestr);
      srsLOG_PRINT("Setting added time for %s to %s", id, timestr);
      srsFile_Create("added.txt");
      srsFile_SetContent("added.txt", timestr);

      srsTime_ToString(scheduled, timestr);
      srsLOG_PRINT("Setting scheduled time for %s to %s", id, timestr);
      srsFile_Create("scheduled.txt");
      srsFile_SetContent("scheduled.txt", timestr);

      srsLOG_PRINT("Creating front for %s", id);
      srsFile_Create("front.txt");
      srsFile_SetContent("front.txt", "things");

      srsLOG_PRINT("Creating back for %s", id);
      srsFile_Create("back.txt");
      srsFile_SetContent("back.txt", "stuff");
      srsDir_PopCWD(NULL);
    }
    srsDir_PopCWD(NULL);
  }
  srsLOG_PRINT("Done creating cards");
  /* TODO We may want to utilize this for an ASSERT */
  return true;
}

int comp_card_id(const void *a, const void *b)
{
  srsCARD card_a = *((const srsCARD *)a);
  srsCARD card_b = *((const srsCARD *)b);
  return strcmp(card_a.id, card_b.id);
}

/* A test runs various assertions, then calls PASS(), FAIL(), or SKIP(). */
TEST test_card(void)
{
  char *cwd = strdup(srsDir_GetCWD());
  ASSERT(cwd);

  srsLOG_PRINT("Adding test cards");
  srsTIME now = srsTime_Now();
  size_t added_count = 0;
  add_card("id_0", now, now); added_count++;
  add_card("id_1", now, now); added_count++;
  add_card("id_2", now, now); added_count++;
  add_card("id_3", now, now); added_count++;

  srsDir_SetCWD(cwd);
  free(cwd);

  srsCARD *cards = NULL;
  size_t count = 0;

  /* Null deck path; Null count output */
  srsERROR_CLEAR();
  ASSERT_EQ(NULL, srsCard_GetAll(NULL, NULL));
  ASSERT_EQ(srsFAIL, srsError_Get().code);

  /* Null deck path; Valid count output */
  srsERROR_CLEAR();
  ASSERT_EQ(NULL, srsCard_GetAll(NULL, &count));
  ASSERT_EQ(srsFAIL, srsError_Get().code);

  /* Valid deck path; Null count output */
  srsERROR_CLEAR();
  ASSERT_EQ(NULL, srsCard_GetAll(".", NULL));
  ASSERT_EQ(srsFAIL, srsError_Get().code);

  /* Missing deck path */
  srsERROR_CLEAR();
  ASSERT_EQ(NULL, srsCard_GetAll("missing-deck-path", &count));
  ASSERT_EQ(srsFAIL, srsError_Get().code);

  /* Invalid deck path */
  srsERROR_CLEAR();
  ASSERT_EQ(NULL, srsCard_GetAll("..", &count));
  ASSERT_EQ(srsFAIL, srsError_Get().code);

  /* Ok, time for perfect world */
  srsLOG_PRINT("Running srsCard_GetAll");
  cards = srsCard_GetAll(".", &count);

  srsLOG_PRINT("Checking card count");
  ASSERT_EQ(added_count, count);

  srsLOG_PRINT("Sorting card list");
  qsort((void *)cards, count, sizeof(*cards), comp_card_id);

  srsLOG_PRINT("Checking card list");
  ASSERT_STR_EQ("id_0", cards[0].id);
  ASSERT_STR_EQ("id_1", cards[1].id);
  ASSERT_STR_EQ("id_2", cards[2].id);
  ASSERT_STR_EQ("id_3", cards[3].id);

  srsLOG_PRINT("Checking card added/scheduled times");
  size_t i = 0;
  for (i = 0; i < count; i++)
  {
    const char *dir = srsDir_PushCWD(cards[i].path);
    ASSERT(strstr(dir, cards[i].id) != NULL);
    ASSERT(srsFile_Exists("added.txt"));
    ASSERT(srsFile_Exists("scheduled.txt"));
    ASSERT_MEM_EQ(&now, &cards[i].when_added, sizeof(now));
    ASSERT_MEM_EQ(&now, &cards[i].when_next_scheduled, sizeof(now));
    srsDir_PopCWD(NULL);
  }

  /* TODO test path */
  ASSERT_EQ(NULL, srsCard_GetContent(cards[0], NULL));

  srsLOG_PRINT("Checking front text for card %s: @%s", cards[0].id, cards[0].path);
  char *front = srsCard_GetContent(cards[0], "front.txt");
  ASSERT(front != NULL);
  ASSERT_STR_EQ("things", front);
  free(front);

  srsLOG_PRINT("Checking back text for card %s: @%s", cards[0].id, cards[0].path);
  char *back = srsCard_GetContent(cards[0], "back.txt");
  ASSERT(back != NULL);
  ASSERT_STR_EQ("stuff", back);
  free(back);

  /** TODO Test the content of added/scheduled.txt against now */

  srsLOG_PRINT("Freeing card list");
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

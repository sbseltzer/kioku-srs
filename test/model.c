#include "greatest.h"
#include "kioku/model.h"
#include "kioku/filesystem.h"

static bool createdeck()
{
  bool ok = true;
  ok = ok && kioku_filesystem_create("deck-test/.at");
  ok = ok && kioku_filesystem_create("deck-test/.schedule");
  ok = ok && srsFile_SetContent("deck-test/.at", "3");
  ok = ok && srsFile_SetContent("deck-test/.schedule", "ab\nabcd\nabcdef\na");
  return ok;
}
static bool deletedeck()
{
  bool ok = true;
  ok = ok && kioku_filesystem_delete("deck-test/.at");
  ok = ok && kioku_filesystem_delete("deck-test/.schedule");
  ok = ok && kioku_filesystem_delete("deck-test");
  return ok;
}
static bool createcards()
{
  bool ok = true;
  ok = ok && kioku_filesystem_create("deck-test/cards/a.txt");
  ok = ok && kioku_filesystem_create("deck-test/cards/ab.txt");
  ok = ok && kioku_filesystem_create("deck-test/cards/abcdef.txt");
  return ok;
}
static bool deletecards()
{
  bool ok = true;
  ok = ok && kioku_filesystem_delete("deck-test/cards/a.txt");
  ok = ok && kioku_filesystem_delete("deck-test/cards/ab.txt");
  ok = ok && kioku_filesystem_delete("deck-test/cards/abcdef.txt");
  return ok;
}
/* A test runs various assertions, then calls PASS(), FAIL(), or SKIP(). */
TEST test_card_getpath(void)
{
  deletedeck();
  ASSERT(createdeck());

  /* ASSERT_FALSE(srsModel_Card_GetPath("deck-test", 0)); */
  char path[kiokuPATH_MAX] = {0};

#define deckpath "deck-test/"
#define ext ".txt"

  srsModel_Card_GetPath(deckpath, "a", path, sizeof(path));
  ASSERT_STR_EQ(deckpath "a" ext, path);

  srsModel_Card_GetPath("deck-test", "ab", path, sizeof(path));
  ASSERT_STR_EQ(deckpath "ab" ext, path);

  srsModel_Card_GetPath("deck-test", "abcdef", path, sizeof(path));
  ASSERT_STR_EQ(deckpath "abcdef" ext, path);

  ASSERT(deletedeck());
  PASS();
}

TEST test_card_getnextid(void)
{
  deletedeck();
  ASSERT(createdeck());

  char path[kiokuPATH_MAX + 1] = {0};

  srsFile_SetContent("deck-test/.at", "0");
  ASSERT_FALSE(srsModel_Card_GetNextID("deck-test", path, sizeof(path)));

  srsFile_SetContent("deck-test/.at", "-1");
  ASSERT_FALSE(srsModel_Card_GetNextID("deck-test", path, sizeof(path)));

  srsFile_SetContent("deck-test/.at", "1");
  ASSERT(srsModel_Card_GetNextID("deck-test", path, sizeof(path)));
  ASSERT_STR_EQ("ab", path);

  srsFile_SetContent("deck-test/.at", "2");
  ASSERT(srsModel_Card_GetNextID("deck-test", path, sizeof(path)));
  ASSERT_STR_EQ("abcd", path);

  srsFile_SetContent("deck-test/.at", "3");
  ASSERT(srsModel_Card_GetNextID("deck-test", path, sizeof(path)));
  ASSERT_STR_EQ("abcdef", path);

  srsFile_SetContent("deck-test/.at", "4");
  ASSERT(srsModel_Card_GetNextID("deck-test", path, sizeof(path)));
  ASSERT_STR_EQ("a", path);

  srsFile_SetContent("deck-test/.at", "7");
  ASSERT_FALSE(srsModel_Card_GetNextID("deck-test", path, sizeof(path)));

  ASSERT(deletedeck());
  PASS();
}

/* Suites can group multiple tests with common setup. */
SUITE(the_suite) {
  RUN_TEST(test_card_getpath);
  RUN_TEST(test_card_getnextid);
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

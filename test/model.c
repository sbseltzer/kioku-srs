#include "greatest.h"
#include "kioku/git.h"
#include "kioku/model.h"
#include "kioku/error.h"
#include "kioku/filesystem.h"

static bool createdeck()
{
  bool ok = true;
  ok = ok && srsFile_Create("deck-test/.at");
  ok = ok && srsFile_Create("deck-test/.schedule");
  ok = ok && srsFile_SetContent("deck-test/.at", "3");
  ok = ok && srsFile_SetContent("deck-test/.schedule", "ab\nabcd\nabcdef\na");
  return ok;
}
static bool deletedeck()
{
  bool ok = true;
  ok = ok && srsPath_Remove("deck-test/.at");
  ok = ok && srsPath_Remove("deck-test/.schedule");
  ok = ok && srsPath_Remove("deck-test");
  return ok;
}
static bool createcards()
{
  bool ok = true;
  ok = ok && srsFile_Create("deck-test/cards/a.txt");
  ok = ok && srsFile_Create("deck-test/cards/ab.txt");
  ok = ok && srsFile_Create("deck-test/cards/abcdef.txt");
  return ok;
}
static bool deletecards()
{
  bool ok = true;
  ok = ok && srsPath_Remove("deck-test/cards/a.txt");
  ok = ok && srsPath_Remove("deck-test/cards/ab.txt");
  ok = ok && srsPath_Remove("deck-test/cards/abcdef.txt");
  return ok;
}

TEST test_get_set_root(void)
{
  /* Root should be initialized to NULL */
  ASSERT_EQ(NULL, srsModel_GetRoot());

  /* Test against NULL path */
  ASSERT_EQ(srsOK, srsModel_SetRoot(NULL));
  /* Test against non-existing path */
  ASSERT_EQ(srsFAIL, srsModel_SetRoot("not/a/path"));
  /* Test against file path */
  ASSERT_EQ(true, srsFile_Create("is-a-file"));
  ASSERT_EQ(srsFAIL, srsModel_SetRoot("is-a-file"));
  /* Test against a non-repository */
  ASSERT_EQ(true, srsDir_Create("not-a-git-repo"));
  ASSERT_EQ(srsFAIL, srsModel_SetRoot("not-a-git-repo"));
  /* Test against a repository */

  const srsGIT_CREATE_OPTS opts = srsGIT_CREATE_OPTS_INIT;
  ASSERT_EQ(true, srsGit_Repo_Create("a-git-repo", opts));
  ASSERT_EQ(srsFAIL, srsModel_SetRoot("a-git-repo"));

  ASSERT_EQ(srsOK, srsModel_CreateRoot("a-model"));
  ASSERT_EQ(srsOK, srsModel_SetRoot("a-model"));
  const char *git_full_path = srsGit_Repo_GetCurrent();
  ASSERT(git_full_path);
  ASSERT(git_full_path[0] != '\0');
  /*
    The pointer returned by srsGit_Repo_GetCurrent is not owned by us, and could be invalidated by further API calls.
    I had a problem before where my SetRoot implementation closing/opening a new repository invalidated the result of srsGit_Repo_GetCurrent.
    To mitigate this, we copy the path here before doing any checks.
    I'm thinking that exposing a user to that kind of problem is sign of design flaw, so I'm going to submit an issue for it.
  */
  char expected_path[4096] = {0};
  strcpy(expected_path, git_full_path);
  ASSERT_STR_EQ(expected_path, srsModel_GetRoot());
  PASS();
}

/* A test runs various assertions, then calls PASS(), FAIL(), or SKIP(). */
TEST test_card_getpath(void)
{
  deletedeck();
  ASSERT(createdeck());

  /* ASSERT_FALSE(srsModel_Card_GetPath("deck-test", 0)); */
  char path[srsPATH_MAX] = {0};

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

  char path[srsPATH_MAX + 1] = {0};

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

TEST TestExistsInRoot(void)
{
  srsModel_SetRoot(NULL);

  /* Test some input before root is set */
  srsError_Reset();
  ASSERT_EQ(false, srsModel_ExistsInRoot(NULL));
  ASSERT_EQ(srsE_API, srsError_Get().code);

  srsError_Reset();
  ASSERT_EQ(false, srsModel_ExistsInRoot("abc"));
  ASSERT_EQ(srsE_API, srsError_Get().code);

  srsError_Reset();
  ASSERT_EQ(false, srsModel_ExistsInRoot(".."));
  ASSERT_EQ(srsE_API, srsError_Get().code);

  srsError_Reset();
  ASSERT_EQ(false, srsModel_ExistsInRoot(TESTDIR"/path/to/root"));
  ASSERT_EQ(srsE_API, srsError_Get().code);

  /* Make a repo */
  srsModel_CreateAndSetRoot(TESTDIR"/path/to/root");

  /* Test null input */
  srsError_Reset();
  ASSERT_EQ(false, srsModel_ExistsInRoot(NULL));
  ASSERT_EQ(srsE_INPUT, srsError_Get().code);

  const char *root = srsModel_GetRoot();
  ASSERT(root);
  srsLOG_PRINT("Root = %s", root);
  srsLOG_PRINT("Test Root = %s", TESTDIR"/path/to/root");
  /* Test various forms of the root path */
  ASSERT_EQ(false,  srsModel_ExistsInRoot(root));
  ASSERT_EQ(false,  srsModel_ExistsInRoot(TESTDIR"//path/to/root"));
  ASSERT_EQ(false,  srsModel_ExistsInRoot(TESTDIR"/path/to/root/"));
  ASSERT_EQ(false,  srsModel_ExistsInRoot(TESTDIR"/path/to/root/../root"));
  ASSERT_EQ(false,  srsModel_ExistsInRoot(TESTDIR"/path/to/root/../root/"));
  /* Test out of root paths */
  ASSERT_EQ(false,  srsModel_ExistsInRoot(TESTDIR));
  /* Test something that looks like root path but would misbehave for naive strcmp implementations */
  srsFile_Create(TESTDIR"path/to/rootabc/def");
  ASSERT_EQ(true, srsFile_Exists(TESTDIR"path/to/rootabc/def"));
  ASSERT_EQ(false, srsModel_ExistsInRoot(TESTDIR"path/to/rootabc/def"));

  /* Test valid absolute paths */
  srsFile_Create(TESTDIR"/path/to/root/abc/def");
  ASSERT_EQ(true, srsFile_Exists(TESTDIR"/path/to/root/abc/def"));
  ASSERT_EQ(true,  srsModel_ExistsInRoot(TESTDIR"/path/to/root/abc/"));
  ASSERT_EQ(true,  srsModel_ExistsInRoot(TESTDIR"/path/to/root/abc/def"));
  ASSERT_EQ(true,  srsModel_ExistsInRoot(TESTDIR"/path/to/root/abc"));

  /* Test valid relative paths */
  srsDir_SetCWD(root);
  ASSERT_EQ(true,  srsModel_ExistsInRoot("./abc"));
  ASSERT_EQ(true,  srsModel_ExistsInRoot("./abc/"));
  ASSERT_EQ(true,  srsModel_ExistsInRoot("./abc/def"));
  ASSERT_EQ(true,  srsModel_ExistsInRoot("abc"));
  ASSERT_EQ(true,  srsModel_ExistsInRoot("abc/def"));

  /* Test invalid relative paths */
  ASSERT_EQ(false,  srsModel_ExistsInRoot("."));
  ASSERT_EQ(false,  srsModel_ExistsInRoot("./abc/ghi"));
  ASSERT_EQ(false,  srsModel_ExistsInRoot("abc/ghi"));
  ASSERT_EQ(false,  srsModel_ExistsInRoot("../root"));
  ASSERT_EQ(false,  srsModel_ExistsInRoot(".."));
  ASSERT_EQ(true, srsFile_Create(TESTDIR"path/to/notroot"));
  ASSERT_EQ(false,  srsModel_ExistsInRoot("../notroot"));

  /* Test more valid relative paths that LOOKED like they could've been INVALID */
  ASSERT_EQ(true,  srsModel_ExistsInRoot("../root/abc"));
  ASSERT_EQ(true,  srsModel_ExistsInRoot("../root/abc/def"));
  ASSERT_EQ(true,  srsModel_ExistsInRoot("../notroot/../root/abc/def"));
  PASS();
}

/* Suites can group multiple tests with common setup. */
SUITE(the_suite) {
  RUN_TEST(test_card_getpath);
  RUN_TEST(test_card_getnextid);
  RUN_TEST(test_get_set_root);
  RUN_TEST(TestExistsInRoot);
}

/* Add definitions that need to be in the test runner's main file. */
GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
  GREATEST_MAIN_BEGIN();      /* command-line options, initialization. */

  /* Individual tests can be run directly. */
  /* RUN_TEST(x_should_equal_1); */

  /* Tests can also be gathered into test suites. */
  RUN_SUITE(the_suite);

  srsERROR_LOG();

  GREATEST_MAIN_END();        /* display results */
}

#include "greatest.h"
#include "kioku/git.h"
#include "kioku/filesystem.h"

/* A test runs various assertions, then calls PASS(), FAIL(), or SKIP(). */
TEST git_create_makes_a_repository(void)
{
  #define REPO_NAME "repo"
  const char *writeme = "me\n";
  srsGIT_CREATE_OPTS opts = {"username", writeme, "Initial commit, dawg"};
  ASSERT(srsGit_Repo_Create(REPO_NAME, opts));
  ASSERT_EQ_FMT(1, srsGit_InitCount(), "Still has %d instances");
  ASSERT(srsDir_Exists(REPO_NAME "/.git/"));
  ASSERT(srsFile_Exists(REPO_NAME "/username"));
  ASSERT(srsGit_IsRepo(REPO_NAME));

  ASSERT_EQ_FMT(1, srsGit_InitCount(), "Still has %d instances");

  char content[2086] = {0};

  srsFile_GetContent(REPO_NAME "/username", content, sizeof(content));
  ASSERT_STR_EQ(writeme, content);

  srsFile_GetContent(REPO_NAME "/.git/HEAD", content, sizeof(content));
  const char *headref = "ref: refs/heads/master\n";
  ASSERT_STR_EQ(headref, content);

  ASSERT(srsFile_GetContent(REPO_NAME "/.git/logs/HEAD", content, sizeof(content)));
  const char *commitlog = "Me <me@example.com> 1507741820 -0400  commit (initial): Initial commit, dawg\n";
  const char *commitlog_me = strstr(commitlog, "Me");
  const char *content_me = strstr(content, "Me");
  ASSERT(commitlog_me != NULL);
  ASSERT(content_me != NULL);
  ASSERT_STRN_EQ(commitlog_me, content_me, strlen("Me <me@example.com>"));
  const char *commitlog_commit = strstr(commitlog, "commit");
  const char *content_commit = strstr(content, "commit");
  ASSERT(commitlog_commit != NULL);
  ASSERT(content_commit != NULL);
  ASSERT_STR_EQ(commitlog_commit, content_commit);

  ASSERT_EQ_FMT(srsGit_InitCount(), 1, "Still has %d instances");
  ASSERT(srsGit_Shutdown());

  ASSERT(srsGit_IsRepo(REPO_NAME));
  PASS();
}

/* Suites can group multiple tests with common setup. */
SUITE(the_suite) {
  RUN_TEST(git_create_makes_a_repository);
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

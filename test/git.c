#include "greatest.h"
#include "kioku/simplegit.h"
#include "kioku/filesystem.h"

/* A test runs various assertions, then calls PASS(), FAIL(), or SKIP(). */
TEST git_create_makes_a_repository(void) {
  #define REPO_NAME "repo"
  const char *writeme = "me\n";
  srsGIT_CREATE_OPTS opts = {"username", writeme, "Initial commit, dawg"};
  ASSERT(srsGit_Repo_Create(REPO_NAME, opts));
  ASSERT(kioku_filesystem_exists(REPO_NAME "/.git/"));
  ASSERT(kioku_filesystem_exists(REPO_NAME "/username"));

  char content[2086] = {0};

  kioku_filesystem_getcontent(REPO_NAME "/username", content, sizeof(content));
  ASSERT_STR_EQ(writeme, content);

  kioku_filesystem_getcontent(REPO_NAME "/.git/HEAD", content, sizeof(content));
  const char *headref = "ref: refs/heads/master\n";
  ASSERT_STR_EQ(headref, content);

  kioku_filesystem_getcontent(REPO_NAME "/.git/logs/HEAD", content, sizeof(content));
  const char *commitlog = "Me <me@example.com> 1507741820 -0400  commit (initial): Initial commit, dawg\n";
  ASSERT_STRN_EQ(strstr(commitlog, "Me"), strstr(content, "Me"), strlen("Me <me@example.com>"));
  ASSERT_STR_EQ(strstr(commitlog, "commit"), strstr(content, "commit"));

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

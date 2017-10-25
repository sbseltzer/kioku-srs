#include "greatest.h"

#include "kioku/filesystem.h"
#include <string.h>

TEST test_up_path(void)
{
  printf("Testing kioku_path_up_index..."kiokuSTRING_LF);
  /* Test full path traversal */
  char path[] = "a/ab/abc/abcd/abcde.txt";
  int32_t index = kioku_path_up_index(path, -1);
  ASSERT(index > 0);
  ASSERT_EQ_FMT(13, index, "%d");
  ASSERT_EQ_FMT('/', path[index], "%c");
  path[index] = '\0';
  ASSERT_STR_EQ("a/ab/abc/abcd", path);

  index = kioku_path_up_index(path, index);
  ASSERT(index > 0);
  ASSERT_EQ('/', path[index]);
  ASSERT(index == 8);
  path[index] = '\0';
  ASSERT_STR_EQ("a/ab/abc", path);

  index = kioku_path_up_index(path, index);
  ASSERT(index > 0);
  ASSERT_EQ('/', path[index]);
  ASSERT(index == 4);
  path[index] = '\0';
  ASSERT_STR_EQ("a/ab", path);

  index = kioku_path_up_index(path, index);
  ASSERT_EQ_FMT('/', path[index], "%c");
  ASSERT(index == 1);
  path[index] = '\0';
  ASSERT_STR_EQ("a", path);

  index = kioku_path_up_index(path, index);
  ASSERT_EQ_FMT(-1, index, "%d");

  /* Test some potential edge cases */
  index = kioku_path_up_index(NULL, -1);
  ASSERT(index == -1);

  index = kioku_path_up_index("", -1);
  ASSERT(index == -1);

  index = kioku_path_up_index("abcdefg", -1);
  ASSERT(index == -1);

  index = kioku_path_up_index("/", -1);
  ASSERT(index == -1);
  PASS();
}

TEST test_trim_path(void)
{
  printf("Testing kioku_path_trimpoints..."kiokuSTRING_LF);
  const char *path;
  size_t start;
  size_t end;

  path = NULL;
  kioku_path_trimpoints(path, &start, &end);
  ASSERT(start == 0);
  ASSERT(end == 0);

  path = "";
  kioku_path_trimpoints(path, &start, &end);
  ASSERT(start == 0);
  ASSERT(end == 0);

  path = "/path";
  kioku_path_trimpoints(path, &start, &end);
  ASSERT(start == 0);
  ASSERT(end == 4);

  path = "/path/";
  kioku_path_trimpoints(path, &start, &end);
  ASSERT(start == 0);
  ASSERT(end == 4);
  ASSERT(strncmp(path + start, "/path", end - start) == 0);

  path = "///path//";
  kioku_path_trimpoints(path, &start, &end);
  ASSERT(start == 2);
  ASSERT(end == start + 4);
  ASSERT(strncmp(path + start, "/path", end - start) == 0);
  PASS();
}

#define DESTSIZE 16
TEST test_concat_path(void)
{
  printf("Testing kioku_path_concat..."kiokuSTRING_LF);
  const char *path1;
  const char *path2;
  const char *expected_path;
  int32_t got_len;
  char dest[DESTSIZE] = {0};

  /* Test empty paths */
  path1 = "";
  path2 = "";
  expected_path = "";
  got_len = kioku_path_concat(dest, sizeof(dest), path1, path2);
  ASSERT(strlen(dest) == got_len);
  ASSERT(strlen(expected_path) == got_len);
  ASSERT_STR_EQ(dest, expected_path);
  ASSERT(got_len < DESTSIZE - 1);
  ASSERT(strlen(dest) < DESTSIZE);

  /* Test empty path1 */
  path1 = "";
  path2 = "path/1";
  expected_path = "path/1";
  got_len = kioku_path_concat(dest, sizeof(dest), path1, path2);
  ASSERT(strlen(dest) == got_len);
  ASSERT(strlen(expected_path) == got_len);
  ASSERT_STR_EQ(dest, expected_path);
  ASSERT(got_len < DESTSIZE - 1);
  ASSERT(strlen(dest) < DESTSIZE);

  /* Test empty path2 */
  path1 = "/test";
  path2 = "";
  expected_path = "/test";
  got_len = kioku_path_concat(dest, sizeof(dest), path1, path2);
  ASSERT(strlen(dest) == got_len);
  ASSERT(strlen(expected_path) == got_len);
  ASSERT_STR_EQ(dest, expected_path);
  ASSERT(got_len < DESTSIZE - 1);
  ASSERT(strlen(dest) < DESTSIZE);

  /* Test perfect input */
  path1 = "/test/";
  path2 = "path/1";
  expected_path = "/test/path/1";
  got_len = kioku_path_concat(dest, sizeof(dest), path1, path2);
  ASSERT(strlen(dest) == got_len);
  ASSERT(strlen(expected_path) == got_len);
  ASSERT_STR_EQ(dest, expected_path);
  ASSERT(got_len < DESTSIZE - 1);
  ASSERT(strlen(dest) < DESTSIZE);

  /* Test removal of redundant slashes */
  path1 = "///test//";
  path2 = "//path/1///";
  got_len = kioku_path_concat(dest, sizeof(dest), path1, path2);
  ASSERT(strlen(dest) == got_len);
  ASSERT(strlen(expected_path) == got_len);
  ASSERT_STR_EQ(dest, expected_path);
  ASSERT(got_len < DESTSIZE - 1);
  ASSERT(strlen(dest) < DESTSIZE);

  /* Test adding necessary slashes */
  path1 = "/test";
  path2 = "path/1";
  got_len = kioku_path_concat(dest, sizeof(dest), path1, path2);
  ASSERT(strlen(dest) == got_len);
  ASSERT(strlen(expected_path) == got_len);
  ASSERT_STR_EQ(dest, expected_path);
  ASSERT(got_len < DESTSIZE - 1);
  ASSERT(strlen(dest) < DESTSIZE);

  /* Test adding a very small path */
  path1 = "/";
  path2 = "1";
  expected_path = "/1";
  got_len = kioku_path_concat(dest, sizeof(dest), path1, path2);
  ASSERT(strlen(dest) == got_len);
  ASSERT(strlen(expected_path) == got_len);
  ASSERT_STR_EQ(dest, expected_path);
  ASSERT(got_len < DESTSIZE - 1);
  ASSERT(strlen(dest) < DESTSIZE);

  /* Test insufficient size */
  path1 = "/test/much/";
  path2 = "/longer/path/1";
  expected_path = "/test/much/longer/path/1";
  got_len = kioku_path_concat(dest, sizeof(dest), path1, path2);
  ASSERT(strlen(expected_path) == got_len);
  ASSERT(strcmp(dest, expected_path) != 0);
  ASSERT(strncmp(dest, expected_path, sizeof(dest) - 1) == 0);
  ASSERT(got_len > DESTSIZE - 1);
  ASSERT(strlen(dest) == DESTSIZE - 1);

  /* Test various valid ways of size probing */
  got_len = kioku_path_concat(dest, 0, path1, path2);
  ASSERT(strlen(expected_path) == got_len);
  ASSERT(got_len > DESTSIZE - 1);
  got_len = kioku_path_concat(NULL, 0, path1, path2);
  ASSERT(strlen(expected_path) == got_len);
  ASSERT(got_len > DESTSIZE - 1);
  got_len = kioku_path_concat(NULL, sizeof(dest), path1, path2);
  ASSERT(strlen(expected_path) == got_len);
  ASSERT(got_len > DESTSIZE - 1);
  /* This one ASSERT_STR_EQ a reasonably portable way to try and tease out a segfault */
  printf("Will it segfault?"kiokuSTRING_LF);
  got_len = kioku_path_concat((char*)-1, 0, path1, path2);
  printf("Nope. Hooray!"kiokuSTRING_LF);
  ASSERT(strlen(expected_path) == got_len);
  ASSERT(got_len > DESTSIZE - 1);

  /* Test dangerous append prevention */
  dest[0] = '.';
  dest[1] = '\0';
  path1 = dest;
  path2 = "dont/append";
  expected_path = "./dont/append";
  got_len = kioku_path_concat(dest, sizeof(dest), path1, path2);
  ASSERT(got_len < 0);
  ASSERT(strcmp(dest, expected_path) != 0);
  ASSERT_STR_EQ(".", dest);

  /* Test dangerous prepend prevention */
  path1 = "dont/prepend";
  path2 = dest;
  expected_path = "dont/prepend/.";
  got_len = kioku_path_concat(dest, sizeof(dest), path1, path2);
  ASSERT(got_len < 0);
  ASSERT(strcmp(dest, expected_path) != 0);
  ASSERT_STR_EQ(".", dest);
  PASS();
}

TEST test_file_manage(void) {
  printf("Testing file management functions..."kiokuSTRING_LF);
  ASSERT(kioku_filesystem_exists(NULL) == false);
  ASSERT(kioku_filesystem_create(NULL) == false);
  ASSERT(kioku_filesystem_rename(NULL, NULL) == false);
  ASSERT(kioku_filesystem_rename(NULL, "a") == false);
  ASSERT(kioku_filesystem_rename("a", NULL) == false);
  ASSERT(kioku_filesystem_isdir(NULL) == false);
  ASSERT(kioku_filesystem_delete(NULL) == false);

  const char *path = "a/b/c/a.txt";
  ASSERT(kioku_filesystem_exists(path) == false);
  ASSERT(kioku_filesystem_isdir(path) == false);
  ASSERT(kioku_filesystem_create(path));
  ASSERT(kioku_filesystem_exists(path));
  ASSERT(kioku_filesystem_isdir(path) == false);
  ASSERT(kioku_filesystem_create(path) == false);

  ASSERT(kioku_filesystem_delete(path));
  ASSERT(kioku_filesystem_exists(path) == false);
  ASSERT(kioku_filesystem_delete(path) == false);

  ASSERT(kioku_filesystem_exists("a"));
  ASSERT(kioku_filesystem_isdir("a"));
  ASSERT(kioku_filesystem_delete("a") == false);
  ASSERT(kioku_filesystem_exists("a"));

  ASSERT(kioku_filesystem_delete("a/b") == false);
  ASSERT(kioku_filesystem_exists("a/b"));
  ASSERT(kioku_filesystem_isdir("a/b"));

  ASSERT(kioku_filesystem_isdir("a/b/c"));
  ASSERT(kioku_filesystem_delete("a/b/c"));
  ASSERT(kioku_filesystem_exists("a/b/c") == false);
  ASSERT(kioku_filesystem_isdir("a/b/c") == false);
  ASSERT(kioku_filesystem_delete("a/b/c") == false);

  ASSERT(kioku_filesystem_delete("a/b"));
  ASSERT(kioku_filesystem_exists("a/b") == false);
  ASSERT(kioku_filesystem_delete("a/b") == false);

  ASSERT(kioku_filesystem_delete("a"));
  ASSERT(kioku_filesystem_exists("a") == false);
  ASSERT(kioku_filesystem_delete("a") == false);

  /* Test a file-only rename */
  ASSERT(kioku_filesystem_create(path));
  const char *newpath = "a/b/c/d.txt";
  ASSERT(kioku_filesystem_exists(newpath) == false);
  ASSERT(kioku_filesystem_rename(path, newpath));
  ASSERT(kioku_filesystem_exists(path) == false);
  ASSERT(kioku_filesystem_exists(newpath));

  /* Test a directory-only rename */
  path = "a/b/c";
  newpath = "a/b/d";
  ASSERT(kioku_filesystem_exists(path));
  ASSERT(kioku_filesystem_exists(newpath) == false);
  ASSERT(kioku_filesystem_rename(path, newpath));
  ASSERT(kioku_filesystem_exists(path) == false);
  ASSERT(kioku_filesystem_exists(newpath));

  /* Test a move-like rename */
  path = "a/b/d/d.txt";
  newpath = "a/b/c/d.txt";
  ASSERT(kioku_filesystem_exists(path));
  ASSERT(kioku_filesystem_exists(newpath) == false);
  ASSERT(kioku_filesystem_rename(path, newpath) == false);
  ASSERT(kioku_filesystem_exists(path));
  ASSERT(kioku_filesystem_exists(newpath) == false);

  /* Cleanup */
  ASSERT(kioku_filesystem_delete(path));
  ASSERT(kioku_filesystem_delete("a/b/d"));
  ASSERT(kioku_filesystem_delete("a/b"));
  ASSERT(kioku_filesystem_delete("a"));

  char fullpath[255] = {0};
  kioku_path_concat(fullpath, sizeof(fullpath), BUILDDIR, "a/b/c/a.txt");
  ASSERT(kioku_filesystem_exists(fullpath) == false);
  ASSERT(kioku_filesystem_create(fullpath));
  ASSERT(kioku_filesystem_exists(fullpath));
  ASSERT(kioku_filesystem_create(fullpath) == false);
  ASSERT(kioku_filesystem_delete(fullpath));
  ASSERT(kioku_filesystem_exists(fullpath) == false);

  kioku_path_concat(fullpath, sizeof(fullpath), BUILDDIR, "a/b/c");
  ASSERT(kioku_filesystem_delete(fullpath));
  ASSERT(kioku_filesystem_exists(fullpath) == false);
  ASSERT(kioku_filesystem_delete(fullpath) == false);

  kioku_path_concat(fullpath, sizeof(fullpath), BUILDDIR, "a/b");
  ASSERT(kioku_filesystem_delete(fullpath));
  ASSERT(kioku_filesystem_exists(fullpath) == false);
  ASSERT(kioku_filesystem_delete(fullpath) == false);

  kioku_path_concat(fullpath, sizeof(fullpath), BUILDDIR, "a");
  ASSERT(kioku_filesystem_delete(fullpath));
  ASSERT(kioku_filesystem_exists(fullpath) == false);
  ASSERT(kioku_filesystem_delete(fullpath) == false);
  PASS();
}

TEST test_file_readlinenumber(void)
{
  printf("Testing line number reading..."kiokuSTRING_LF);
  const char *forms[] = {
    "\n",                       /*  1 */
    "%s\n",                     /*  2 */
    "%s\r\n",                   /*  3 */
    "%s\n",                     /*  4 */
    "\n",                       /*  5 */
    "%s\r\n",                   /*  6 */
    "%s\n",                     /*  7 */
    "%s\n",                     /*  8 */
    "%s\n",                     /*  9 */
    "\r%s\n",                   /* 10 */
    "%s\r\n",                   /* 11 */
    "\r\n",                     /* 12 */
    "%s\n",                     /* 13 */
    "%s\n",                     /* 14 */
    "\n"                        /* 15 */
  };
  const char *lines[] = {
    "", /*  1 */
    "duckmeat",                 /*  2 */
    "stuff",                    /*  3 */
    "and",                      /*  4 */
    "", /*  5 */
    "with other",               /*  6 */
    "words",                    /*  7 */
    "and\r\r\r such",           /*  8 */
    "...",                      /*  9 */
    "but it's not over yet",    /* 10 */
    "still 5 lines to go",      /* 11 */
    "", /* 12 */
    "and now 3 including this", /* 13 */
    "and now 2"                 /* 14 */
    "", /* 15 */
  };
  const char *lines_out[] = {
    lines[0],                   /*  1 */
    lines[1],                   /*  2 */
    lines[2],                   /*  3 */
    lines[3],                   /*  4 */
    lines[4],                   /*  5 */
    lines[5],                   /*  6 */
    lines[6],                   /*  7 */
    "and such",                 /*  8 */
    lines[8],                   /*  9 */
    lines[9],                   /* 10 */
    lines[10],                  /* 11 */
    lines[11],                  /* 12 */
    lines[12],                  /* 13 */
    lines[13],                  /* 14 */
    lines[14],                  /* 15 */
  };

  char filebuf[2000] = {0};
  int32_t written = 0;
  size_t index;
  printf("There are %u lines"kiokuSTRING_LF, sizeof(lines));
  size_t numlines = sizeof(lines) / sizeof(*lines);
  for (index = 0; index < numlines; index++)
  {
    written += sprintf(filebuf + written, forms[index], lines[index]);
  }
  printf("File content: %s"kiokuSTRING_LF, filebuf);

  const char *multilinefile = "./testlines";
  kioku_filesystem_delete(multilinefile);
  ASSERT(kioku_filesystem_create(multilinefile));
  ASSERT(kioku_filesystem_setcontent(multilinefile, filebuf));
  char linebuf[256] = {0};
  for (index = 0; index < numlines; index++)
  {
    written = srsFile_ReadLineByNumber(multilinefile, index + 1, linebuf, sizeof(linebuf));
    ASSERT(written >= 0);
    /* Some lines have a number of CRs that are skipped and therefore reduce the length. */
    ASSERT_EQ(strlen(lines_out[index]), written);
    ASSERT_STR_EQ(lines_out[index], linebuf);
    /* @todo test line output */
  }
  /* Test invalid line numbers */
  ASSERT_EQ(-1, srsFile_ReadLineByNumber(multilinefile, 0, linebuf, sizeof(linebuf)));
  ASSERT_EQ(-1, srsFile_ReadLineByNumber(multilinefile, 9000, linebuf, sizeof(linebuf)));
  /* Test missing file */
  ASSERT_EQ(-1, srsFile_ReadLineByNumber("missing", 1, linebuf, sizeof(linebuf)));
  /* Test invalid buffer */
  ASSERT_EQ(-1, srsFile_ReadLineByNumber(multilinefile, 1, NULL, sizeof(linebuf)));
  /* Test invalid buffer size */
  ASSERT_EQ(-1, srsFile_ReadLineByNumber(multilinefile, 1, linebuf, 0));

  /* Cleanup */

  kioku_filesystem_delete(multilinefile);
  PASS();
}

TEST test_file_io(void)
{
  printf("Testing file io functions"kiokuSTRING_LF);
  char buffer[255] = {0};
  printf("\tTesting getlen edges..."kiokuSTRING_LF);
  ASSERT(kioku_filesystem_getlen(NULL) == -1);
  ASSERT(kioku_filesystem_getlen("invalid/file/path.txt") == -1);
  ASSERT(kioku_filesystem_getlen(".") == -1);

  printf("\tTesting setcontent edges..."kiokuSTRING_LF);
  ASSERT(kioku_filesystem_setcontent(NULL, "test") == false);
  ASSERT(kioku_filesystem_setcontent(".", "test") == false);

  printf("\tTesting getcontent edges..."kiokuSTRING_LF);
  ASSERT(kioku_filesystem_getcontent(NULL, NULL, 0) == false);
  ASSERT(kioku_filesystem_getcontent(".", NULL, 0) == false);
  ASSERT(kioku_filesystem_getcontent("invalid/file/path.txt", buffer, sizeof(buffer)) == false);

  const char *filepath = "a/test/file.txt";
  const char *content = "some text";

  kioku_filesystem_delete(filepath);

  printf("\tTesting setcontent/getlen..."kiokuSTRING_LF);
  ASSERT(kioku_filesystem_setcontent(filepath, content) == false);
  kioku_filesystem_create(filepath);
  ASSERT(kioku_filesystem_setcontent(filepath, content));
  ASSERT(kioku_filesystem_getlen(filepath) == strlen(content));

  printf("\tTesting getcontent..."kiokuSTRING_LF);
  ASSERT(kioku_filesystem_getcontent(filepath, buffer, sizeof(buffer)));
  ASSERT(strlen(buffer) == strlen(content));
  ASSERT_STR_EQ(buffer, content);

  printf("\tTesting setcontent/getcontent..."kiokuSTRING_LF);
  content = "some totally different text";
  ASSERT(kioku_filesystem_setcontent(filepath, content));
  ASSERT(kioku_filesystem_getcontent(filepath, buffer, sizeof(buffer)));
  ASSERT(strlen(buffer) == strlen(content));
  ASSERT_STR_EQ(buffer, content);

  /* Cleanup */
  char filepathdup[64] = {0};
  strncpy(filepathdup, filepath, strlen(filepath));
  int32_t upindex = -1;
  while ((upindex = kioku_path_up_index(filepathdup, upindex)) > -1)
  {
    ASSERT(kioku_filesystem_delete(filepathdup));
    filepathdup[upindex] = '\0';
  }
  PASS();
}

/** @todo test whether start_index behaves properly for get up index */
TEST test_resolve_relative(void)
{
  printf("Testing kioku_path_resolve_relative..." kiokuSTRING_LF);
  struct path_pair_s {
    const char in[kiokuPATH_MAX+1];
    const char out[kiokuPATH_MAX+1];
  } paths[] = {
    {".", "."}, /* Maybe this should return "" */
    {"..", ".."},
    {"./", "./"}, /* Maybe this should return "" */
    {"/.", "/"},
    {"C:/.", "C:/"},
    {"../", "../"},
    {"/", "/"},
    {"C:/", "C:/"},
    {"/..", "/"}, /* Obviously cannot go up from a root path - however a user might make a mistake running this method on the second part of a concatenated path before concatenation. Maybe leave it as ASSERT_STR_EQ? */
    {"C:/..", "C:/"}, /* In this case, a drive letter obviously cannot be part of the path, so this ASSERT_STR_EQ probably an OK conversion */
    {"abc/.", "abc/"},
    {"abc/..", ""}, /* Maybe return "." */
    {"abc/../..", ".."},
    {"/abc/..", "/"},
    {"C:/abc/..", "C:/"},
    {"/abc/../..", "/"}, /* As before, maybe leave it as "/.." */
    {"C:/abc/../..", "C:/"}, /* As before, maybe leave it as "C:/.." */
  };
  size_t num_pairs = sizeof(paths);
  size_t i;
  for (i = 0; i < num_pairs; i++)
  {
    printf("Testing conversion of '%s' to '%s'"kiokuSTRING_LF, paths[i].in, paths[i].out);
    char path[kiokuPATH_MAX+1] = {0};
    strncpy(path, paths[i].in, kiokuPATH_MAX);
    /* kioku_path_resolve_relative(path, sizeof(path)); */
    /* ASSERT_STR_EQ(path, paths[i].out); */
  }
}
TEST test_fullpath(void)
{
  printf("Testing kioku_path_getfull..." kiokuSTRING_LF);
  /* ASSERT(); */
}

TEST test_dir_traversal(void)
{
  srsLOG_NOTIFY("Testing dir traversal");
  char original_path[kiokuPATH_MAX] = {0};
  char path[kiokuPATH_MAX] = {0};
  const char *cwd = NULL;
  int32_t up_index = -1;

  /* Test basic getting of CWD */
  {
    cwd = srsDir_GetCurrent();
    ASSERT(cwd != NULL);
    strcpy(original_path, cwd);
    srsLOG_NOTIFY(cwd);
  }
  /* Get the expected parent directory */
  {
    up_index = kioku_path_up_index(cwd, -1);
    ASSERT(up_index > 0);
    strcpy(path, cwd);
    path[up_index] = '\0';
  }
  /* Test result of going up one directory */
  {
    cwd = srsDir_SetCurrent("..");
    ASSERT(cwd != NULL);
    srsLOG_NOTIFY(cwd);
    ASSERT_STR_EQ(path, cwd);
  }
  /* Test getting the current directory against the return value of setting it */
  {
    cwd = srsDir_GetCurrent();
    ASSERT(cwd != NULL);
    ASSERT_STR_EQ(path, cwd);
  }
  /** @todo Test going deeper */

  /* Reset directory */
  {
    cwd = srsDir_SetCurrent(original_path);
    ASSERT(cwd != NULL);
    srsLOG_NOTIFY(cwd);
    strcpy(path, cwd);
  }

  /* Test push/pop */
  {
    /* Pop without anything on the stack */
    cwd = srsDir_PopCurrent();
    ASSERT(cwd == NULL);
    /* Ensure that pop didn't change the CWD */
    cwd = srsDir_GetCurrent();
    ASSERT(cwd != NULL);
    ASSERT_STR_EQ(path, cwd);
    /* Calculate next expected directory change */
    up_index = kioku_path_up_index(cwd, -1);
    ASSERT(up_index > 0);
    strcpy(path, cwd);
    path[up_index] = '\0';
    /* Go up one directory via push */
    cwd = srsDir_PushCurrent("..", NULL);
    ASSERT(cwd != NULL);
    srsLOG_NOTIFY(cwd);
    ASSERT_STR_EQ(path, cwd);
    /* Go up another directory via push*/
    cwd = srsDir_PushCurrent("..", NULL);
    ASSERT(cwd != NULL);
    srsLOG_NOTIFY(cwd);
    ASSERT(strcmp(path, cwd) != 0); /** @todo Use a < or > here as appropriate. */
    /** @todo Go back */
    #if 0
    cwd = srsDir_PopCurrent();
    ASSERT(cwd != NULL);
    srsLOG_NOTIFY(cwd);
    ASSERT_STR_EQ(path, cwd);
    #endif
  }

  /* Test popping several directories */
  /* Test whether setting directory clears the stack */
  /* Test pushing several cycles beyond stack limit and then popping them all */

  PASS();
}

SUITE(test_filesystem) {
  RUN_TEST(test_file_readlinenumber);
  printf(kiokuSTRING_LF);
  RUN_TEST(test_up_path);
  printf(kiokuSTRING_LF);
  RUN_TEST(test_trim_path);
  printf(kiokuSTRING_LF);
  RUN_TEST(test_concat_path);
  printf(kiokuSTRING_LF);
  RUN_TEST(test_file_manage);
  printf(kiokuSTRING_LF);
  RUN_TEST(test_file_io);
  printf(kiokuSTRING_LF);
  RUN_TEST(test_dir_traversal);
}
/* Add definitions that need to be in the test runner's main file. */
GREATEST_MAIN_DEFS();
int main(int argc, char **argv)
{
  GREATEST_MAIN_BEGIN();      /* command-line options, initialization. */

  RUN_SUITE(test_filesystem);

  GREATEST_MAIN_END();        /* display results */
}

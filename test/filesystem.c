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
/** @todo Add tests for srsFile_Exists */
TEST test_file_manage(void) {
  printf("Testing file management functions..."kiokuSTRING_LF);
  const char *path = NULL;
  const char *partial_path = NULL;
  /** @todo tests with trailing and leading separators */
  /** @todo tests with mixed separators */
  /** @todo tests with windows drives */
  /** @todo Consider somehow simplifying these test tables and then have a table OF them that describe different path configurations */
  /* For File-related API functions */
  struct FileTestTable {
    bool CannotCreateExistingFile;
    bool CannotCreateExistingDir;
    bool DoesNotExistBeforeCreation;
    bool ExistsAfterOkCreation;
    bool DoesNotExistAfterNotOkCreation;
    bool CreatedFileIsFile;
    bool CreatedFileIsNotDir;
    bool CanDeleteExisting;
    bool CanDeleteNonExisting;
    bool CanDeleteExistingFile;
    bool CannotDeleteExistingDir;
    bool DirExistsAfterNotOkDeletion;
    bool FileExistsAfterNotOkDeletion;
    bool FileDoesNotExistAfterOkDeletion;
  };
  /* For Dir-related API functions */
  struct DirTestTable {
    bool CannotCreateExistingFile;
    bool CannotCreateExistingDir;
    bool DoesNotExistBeforeCreation;
    bool ExistsAfterOkCreation;
    bool DoesNotExistAfterNotOkCreation;
    bool CreatedDirIsDir;
    bool CreatedDirIsNotFile;
    bool CanDeleteExisting;
    bool CanDeleteNonExisting;
    bool CannotDeleteExistingFile;
    bool CanDeleteExistingEmptyDir;
    bool CanDeleteExistingEmptyDirsRecursively;
    bool CanDeleteExistingNonEmptyDir;
    bool CanDeleteExistingNonEmptyDirsRecursively;
    bool DirExistsAfterNotOkDeletion;
    bool FileExistsAfterNotOkDeletion;
    bool DirDoesNotExistAfterOkDeletion;
  };
  /* Invalid input */
  ASSERT_FALSE(srsDir_Exists(NULL));
  ASSERT_FALSE(srsFile_Exists(NULL));
  ASSERT_FALSE(srsPath_Exists(NULL));
  ASSERT_FALSE(srsFile_Create(NULL));
  ASSERT_FALSE(srsDir_Create(NULL));
  ASSERT_FALSE(srsPath_Move(NULL, NULL));
  ASSERT_FALSE(srsPath_Move(NULL, "a"));
  ASSERT_FALSE(srsPath_Move("a", NULL));
  ASSERT_FALSE(srsPath_Remove(NULL));

  srsPath_Remove("a/b/c/d.txt");
  srsPath_Remove("a/b/c");
  srsPath_Remove("a/b");
  srsPath_Remove("a");

  partial_path = "a/b/";
  /* creating a dir as a file fails */
  ASSERT_FALSE(srsFile_Create(partial_path));

  /* Existence/creation/removal of dirs */
  ASSERT(srsDir_Create(partial_path));
  ASSERT_FALSE(srsDir_Create(partial_path));
  ASSERT(srsDir_Exists(partial_path));
  ASSERT(srsPath_Exists(partial_path));
  ASSERT_FALSE(srsFile_Exists(partial_path));
  ASSERT_FALSE(srsPath_Remove("a/b/c"));
  ASSERT_FALSE(srsPath_Exists("a/b/c"));
  partial_path = "a/b/c";
  ASSERT(srsDir_Create(partial_path));
  ASSERT(srsDir_Exists(partial_path));
  ASSERT(srsPath_Exists(partial_path));
  ASSERT_FALSE(srsFile_Exists(partial_path));
  ASSERT(srsPath_Remove(partial_path));
  ASSERT_FALSE(srsPath_Remove(partial_path));
  ASSERT_FALSE(srsDir_Exists(partial_path));
  /** @todo use the up index function to do these removal/existence tests programatically */
  ASSERT(srsDir_Exists("a/b"));
  ASSERT(srsPath_Remove("a/b"));
  ASSERT_FALSE(srsPath_Remove("a/b"));
  ASSERT_FALSE(srsDir_Exists("a/b"));
  ASSERT(srsDir_Exists("a"));
  ASSERT(srsPath_Remove("a"));
  ASSERT_FALSE(srsPath_Remove("a"));
  ASSERT_FALSE(srsDir_Exists("a"));

  /* Existence/creation/removal of files */
  path = "a/b/c/a.txt";
  ASSERT_FALSE(srsPath_Exists(path));
  ASSERT_FALSE(srsDir_Exists(path));
  ASSERT_FALSE(srsFile_Exists(path));
  ASSERT(srsFile_Create(path));
  ASSERT_FALSE(srsFile_Create(path));
  ASSERT(srsPath_Exists(path));
  ASSERT(srsFile_Exists(path));
  ASSERT_FALSE(srsDir_Exists(path));

  ASSERT(srsPath_Remove(path));
  ASSERT_FALSE(srsPath_Exists(path));
  ASSERT_FALSE(srsFile_Exists(path));
  ASSERT_FALSE(srsPath_Remove(path));

  ASSERT(srsPath_Exists("a"));
  ASSERT(srsDir_Exists("a"));
  ASSERT_FALSE(srsPath_Remove("a"));
  ASSERT(srsPath_Exists("a"));

  ASSERT_FALSE(srsPath_Remove("a/b"));
  ASSERT(srsPath_Exists("a/b"));
  ASSERT(srsDir_Exists("a/b"));

  ASSERT(srsDir_Exists("a/b/c"));
  ASSERT(srsPath_Remove("a/b/c"));
  ASSERT_FALSE(srsPath_Exists("a/b/c"));
  ASSERT_FALSE(srsDir_Exists("a/b/c"));
  ASSERT_FALSE(srsPath_Remove("a/b/c"));

  ASSERT(srsPath_Remove("a/b"));
  ASSERT_FALSE(srsPath_Exists("a/b"));
  ASSERT_FALSE(srsPath_Remove("a/b"));

  ASSERT(srsPath_Remove("a"));
  ASSERT_FALSE(srsPath_Exists("a"));
  ASSERT_FALSE(srsPath_Remove("a"));

  /* Test a file-only rename */
  ASSERT(srsFile_Create(path));
  const char *newpath = "a/b/c/d.txt";
  ASSERT_FALSE(srsPath_Exists(newpath));
  ASSERT(srsPath_Move(path, newpath));
  ASSERT_FALSE(srsPath_Exists(path));
  ASSERT(srsPath_Exists(newpath));

  /* Test a directory-only rename */
  path = "a/b/c";
  newpath = "a/b/d";
  ASSERT(srsPath_Exists(path));
  ASSERT_FALSE(srsPath_Exists(newpath));
  ASSERT(srsPath_Move(path, newpath));
  ASSERT_FALSE(srsPath_Exists(path));
  ASSERT(srsPath_Exists(newpath));

  /* Test a move-like rename */
  path = "a/b/d/d.txt";
  newpath = "a/b/c/d.txt";
  ASSERT(srsPath_Exists(path));
  ASSERT_FALSE(srsPath_Exists(newpath));
  ASSERT_FALSE(srsPath_Move(path, newpath));
  ASSERT(srsPath_Exists(path));
  ASSERT_FALSE(srsPath_Exists(newpath));

  /* Cleanup */
  ASSERT(srsPath_Remove(path));
  ASSERT(srsPath_Remove("a/b/d"));
  ASSERT(srsPath_Remove("a/b"));
  ASSERT(srsPath_Remove("a"));

  char fullpath[255] = {0};
  kioku_path_concat(fullpath, sizeof(fullpath), TESTDIR, "a/b/c/a.txt");
  ASSERT_FALSE(srsPath_Exists(fullpath));
  ASSERT(srsFile_Create(fullpath));
  ASSERT(srsPath_Exists(fullpath));
  ASSERT_FALSE(srsFile_Create(fullpath));
  ASSERT(srsPath_Remove(fullpath));
  ASSERT_FALSE(srsPath_Exists(fullpath));

  kioku_path_concat(fullpath, sizeof(fullpath), TESTDIR, "a/b/c");
  ASSERT(srsPath_Remove(fullpath));
  ASSERT_FALSE(srsPath_Exists(fullpath));
  ASSERT_FALSE(srsPath_Remove(fullpath));

  kioku_path_concat(fullpath, sizeof(fullpath), TESTDIR, "a/b");
  ASSERT(srsPath_Remove(fullpath));
  ASSERT_FALSE(srsPath_Exists(fullpath));
  ASSERT_FALSE(srsPath_Remove(fullpath));

  kioku_path_concat(fullpath, sizeof(fullpath), TESTDIR, "a");
  ASSERT(srsPath_Remove(fullpath));
  ASSERT_FALSE(srsPath_Exists(fullpath));
  ASSERT_FALSE(srsPath_Remove(fullpath));
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
  printf("There are %zu lines"kiokuSTRING_LF, sizeof(lines));
  size_t numlines = sizeof(lines) / sizeof(*lines);
  for (index = 0; index < numlines; index++)
  {
    written += sprintf(filebuf + written, forms[index], lines[index]);
  }
  printf("File content: %s"kiokuSTRING_LF, filebuf);

  const char *multilinefile = "./testlines";
  srsPath_Remove(multilinefile);
  ASSERT(srsFile_Create(multilinefile));
  ASSERT(srsFile_SetContent(multilinefile, filebuf));
  char linebuf[256] = {0};
  for (index = 0; index < numlines; index++)
  {
    written = srsFile_ReadLineByNumber(multilinefile, index + 1, linebuf, sizeof(linebuf));
    ASSERT(written >= 0);
    /* Some lines have a number of CRs that are skipped and therefore reduce the length. */
    ASSERT_EQ(strlen(lines_out[index]), written);
    ASSERT_STR_EQ(lines_out[index], linebuf);
    /** @todo test line output */
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

  srsPath_Remove(multilinefile);
  PASS();
}

TEST test_file_io(void)
{
  printf("Testing file io functions"kiokuSTRING_LF);
  char buffer[255] = {0};
  printf("\tTesting getlen edges..."kiokuSTRING_LF);
  ASSERT(srsFile_GetLength(NULL) == -1);
  ASSERT(srsFile_GetLength("invalid/file/path.txt") == -1);
  ASSERT(srsFile_GetLength(".") == -1);

  printf("\tTesting setcontent edges..."kiokuSTRING_LF);
  ASSERT(srsFile_SetContent(NULL, "test") == false);
  ASSERT(srsFile_SetContent(".", "test") == false);

  printf("\tTesting getcontent edges..."kiokuSTRING_LF);
  ASSERT(srsFile_GetContent(NULL, NULL, 0) == false);
  ASSERT(srsFile_GetContent(".", NULL, 0) == false);
  ASSERT(srsFile_GetContent("invalid/file/path.txt", buffer, sizeof(buffer)) == false);

  const char *filepath = "a/test/file.txt";
  const char *content = "some text";

  srsPath_Remove(filepath);

  printf("\tTesting setcontent/getlen..."kiokuSTRING_LF);
  ASSERT(srsFile_SetContent(filepath, content) == false);
  srsFile_Create(filepath);
  ASSERT(srsFile_SetContent(filepath, content));
  ASSERT(srsFile_GetLength(filepath) == strlen(content));

  printf("\tTesting getcontent..."kiokuSTRING_LF);
  ASSERT(srsFile_GetContent(filepath, buffer, sizeof(buffer)));
  ASSERT(strlen(buffer) == strlen(content));
  ASSERT_STR_EQ(buffer, content);

  printf("\tTesting setcontent/getcontent..."kiokuSTRING_LF);
  content = "some totally different text";
  ASSERT(srsFile_SetContent(filepath, content));
  ASSERT(srsFile_GetContent(filepath, buffer, sizeof(buffer)));
  ASSERT(strlen(buffer) == strlen(content));
  ASSERT_STR_EQ(buffer, content);

  /* Cleanup */
  char filepathdup[64] = {0};
  strncpy(filepathdup, filepath, strlen(filepath));
  int32_t upindex = -1;
  while ((upindex = kioku_path_up_index(filepathdup, upindex)) > -1)
  {
    ASSERT(srsPath_Remove(filepathdup));
    filepathdup[upindex] = '\0';
  }
  PASS();
}

/** @todo test whether start_index behaves properly for get up index */
TEST test_resolve_relative(void)
{
  printf("Testing kioku_path_resolve_relative..." kiokuSTRING_LF);
  struct path_pair_s {
    const char in[srsPATH_MAX+1];
    const char out[srsPATH_MAX+1];
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
    char path[srsPATH_MAX+1] = {0};
    strncpy(path, paths[i].in, srsPATH_MAX);
    /* kioku_path_resolve_relative(path, sizeof(path)); */
    /* ASSERT_STR_EQ(path, paths[i].out); */
  }
}
TEST TestGetFullPath(void)
{
  printf("Testing srsPath_GetFull..." kiokuSTRING_LF);

  /* Test a known existing directory */
  char expected_fullpath[srsPATH_MAX];
  char path[srsPATH_MAX];
  kioku_path_concat(expected_fullpath, sizeof(expected_fullpath), TESTDIR, "a");
  srsLOG_NOTIFY("Check GetFull expecting %s", expected_fullpath);
  ASSERT(0 != srsPath_GetFull("a", path, sizeof(path)));
  srsLOG_NOTIFY("Check GetFull against %s", path);
  /** TODO At some point we must specify whether trailing slashes will be returned by these functions */
  if (expected_fullpath[strlen(expected_fullpath)] == '/')
  {
    expected_fullpath[strlen(expected_fullpath)] = '\0';
  }
  if (path[strlen(path)] == '/')
  {
    path[strlen(path)] = '\0';
  }
  ASSERT_STR_EQ(expected_fullpath, path);

  /* Test a known non-existent directory */
  kioku_path_concat(expected_fullpath, sizeof(expected_fullpath), TESTDIR, "i-do-not-exist");
  srsLOG_NOTIFY("Check GetFull expecting %s", expected_fullpath);
  ASSERT(0 != srsPath_GetFull("i-do-not-exist", path, sizeof(path)));
  srsLOG_NOTIFY("Check GetFull against %s", path);
  /** TODO At some point we must specify whether trailing slashes will be returned by these functions */
  if (expected_fullpath[strlen(expected_fullpath)] == '/')
  {
    expected_fullpath[strlen(expected_fullpath)] = '\0';
  }
  if (path[strlen(path)] == '/')
  {
    path[strlen(path)] = '\0';
  }
  ASSERT_STR_EQ(expected_fullpath, path);

  srsDir_Create("test-up-and-back");
  const char *dir = TESTDIR"/test-up-and-back";
  srsDir_SetCWD(dir);
  ASSERT_EQ(true, srsDir_Exists(dir));
  {
    char buf[srsPATH_MAX] = {0};
    const char *resolveme = "../test-up-and-back";
    srsPath_GetFull(resolveme, buf, sizeof(buf));
    srsLOG_PRINT("Does %s (from %s) resolve to %s?", resolveme, srsDir_GetCWD(), dir);
    ASSERT_STR_EQ(dir, buf);
  }

  PASS();
}

TEST TestSetGetCWD(void)
{
  const char *cwd = NULL;
  char start_path[srsPATH_MAX] = {0};
  char system_cwd[srsPATH_MAX] = {0};

  /* Try getting the CWD and check it against the low-level system CWD */
  ASSERT(srsDir_GetSystemCWD(system_cwd, sizeof(system_cwd)));
  srsLOG_NOTIFY("System CWD: %s", system_cwd);
  cwd = srsDir_GetCWD();
  ASSERT_STR_EQ(system_cwd, cwd);
  /* Store for later check */
  strcpy(start_path, cwd);

  /** @todo do null tests */
  /* Navigate to newly created test directory and check CWD */
  ASSERT(srsFile_Create("./testdir/file"));
  ASSERT(srsDir_SetCWD("./testdir"));
  ASSERT(srsDir_GetSystemCWD(system_cwd, sizeof(system_cwd)));
  srsLOG_NOTIFY("System CWD: %s", system_cwd);
  cwd = srsDir_GetCWD();
  ASSERT_STR_EQ(system_cwd, cwd);

  /* Relative path up to start directory test */
  ASSERT(srsDir_SetCWD(".."));
  cwd = srsDir_GetCWD();
  ASSERT_STR_EQ(start_path, cwd);
  PASS();
}

TEST TestPushPopCWD(void)
{
  srsLOG_NOTIFY("Testing dir traversal");
  const char *dirs[] = {
    "./a",
    "./a/b",
    "./a/b/c",
    "./a/b/d",
    "./a/b/e",
    "./a/b/e/f"
  };
  char start_path[srsPATH_MAX] = {0};
  char path[srsPATH_MAX] = {0};
  const char *cwd = NULL;
  int32_t up_index = -1;

  srsLOG_NOTIFY("NULL tests");
  ASSERT_EQ(NULL,  srsDir_PushCWD(NULL));
  ASSERT_EQ(false, srsDir_PopCWD(NULL));

  /* Clean up directories from previous tests */
  size_t numdirs = sizeof(dirs) / sizeof(dirs[0]);
  srsLOG_NOTIFY("Pre-test cleanup for %zu dirs", numdirs);
  int32_t i = 0;
  for (i = numdirs - 1; i >= 0; i--)
  {
    srsLOG_NOTIFY("Attempting a remove on %s", dirs[i]);
    srsPath_Remove(dirs[i]);
    srsLOG_NOTIFY("Attempted a remove on %s", dirs[i]);
    ASSERT_FALSE(srsPath_Exists(dirs[i]));
  }

  srsLOG_NOTIFY("Attempt to get the CWD");
  cwd = srsDir_GetCWD();
  ASSERT(cwd != NULL);
  srsLOG_NOTIFY("Store cwd %s in buffer", cwd);
  strcpy(start_path, cwd);
  srsLOG_NOTIFY("Stored cwd as %s in buffer", start_path);
  for (i = 0; i < numdirs; i++)
  {
    srsLOG_NOTIFY("Test pushpop for #%d: %s", i, dirs[i]);
    ASSERT(srsDir_Create(dirs[i]));
    /* Grab the full path of the new dir before we change to it. */
    ASSERT(0 != srsPath_GetFull(dirs[i], path, sizeof(path)));
    const char *newdir = srsDir_PushCWD(dirs[i]);
    ASSERT(newdir != NULL);
    /* Check the hypothetical fullpath against the pushed dir path */
    ASSERT_STR_EQ(newdir, path);
    cwd = srsDir_GetCWD();
    ASSERT_STR_EQ(cwd, newdir);
    /* Check the hypothetical fullpath against the cwd path */
    ASSERT_STR_EQ(cwd, path);

    /* Now pop the directory and make sure we returned to the right place */
    ASSERT(srsDir_PopCWD(NULL));
    cwd = srsDir_GetCWD();
    srsLOG_NOTIFY("Check whether %s < %s (%d < 0)", cwd, path, strcmp(cwd, path));
    ASSERT(strcmp(cwd, path) < 0);
    ASSERT_STR_EQ(start_path, cwd);
  }

  /* Test basic getting of CWD */
  {
    cwd = srsDir_GetCWD();
    ASSERT(cwd != NULL);
    strcpy(start_path, cwd);
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
    cwd = srsDir_SetCWD("..");
    ASSERT(cwd != NULL);
    srsLOG_NOTIFY(cwd);
    ASSERT_STR_EQ(path, cwd);
  }
  /* Test getting the current directory against the return value of setting it */
  {
    cwd = srsDir_GetCWD();
    ASSERT(cwd != NULL);
    ASSERT_STR_EQ(path, cwd);
  }
  /** @todo Test going deeper */

  /* Reset directory */
  {
    cwd = srsDir_SetCWD(start_path);
    ASSERT(cwd != NULL);
    srsLOG_NOTIFY(cwd);
    strcpy(path, cwd);
  }

  /* Test pop on empty stack */
  {
    /* Pop everything off the stack */
    while(srsDir_PopCWD(NULL));
    /* Pop without anything on the stack */
    srsLOG_NOTIFY("Pop without anything on the stack");
    ASSERT_FALSE(srsDir_PopCWD(NULL));
    /* Ensure that pop didn't change the CWD */
    cwd = srsDir_GetCWD();
    ASSERT(cwd != NULL);
    ASSERT_STR_EQ(path, cwd);
  }

  /* Test push/pop */
  {
    /* Calculate next expected directory change */
    up_index = kioku_path_up_index(cwd, -1);
    ASSERT(up_index > 0);
    strcpy(path, cwd);
    path[up_index] = '\0';
    srsLOG_NOTIFY("Expecting to go up: %s", path);
    /* Go up one directory via push */
    cwd = srsDir_PushCWD("..");
    ASSERT(cwd != NULL);
    ASSERT_STR_EQ(path, cwd);
    /* Go up another directory via push*/
    cwd = srsDir_PushCWD("..");
    ASSERT(cwd != NULL);
    srsLOG_NOTIFY(cwd);
    ASSERT(strcmp(path, cwd) != 0); /** @todo Use a < or > here as appropriate. */
    /* Go back */
    srsLOG_NOTIFY("This time when we pop, try storing the result");
    char *popped;
    ASSERT(srsDir_PopCWD(&popped));
    ASSERT(popped != NULL);
    srsLOG_NOTIFY("Popped: %s", popped);
    ASSERT_STR_EQ(path, popped);
    free(popped);
    /* Calculate what the directory should have changed to */
    cwd = srsDir_GetCWD();
    srsLOG_NOTIFY(cwd);
    ASSERT(srsDir_PopCWD(&popped));
    ASSERT(popped != NULL);
    srsLOG_NOTIFY("Popped: %s", popped);
    free(popped);
    cwd = srsDir_GetCWD();
    srsLOG_NOTIFY(cwd);
    ASSERT_FALSE(srsDir_PopCWD(&popped));
    ASSERT(popped == NULL);
    cwd = srsDir_GetCWD();
    srsLOG_NOTIFY(cwd);
  }

  /* Test popping several directories */
  /* Test whether setting directory clears the stack */
  /* Test pushing several cycles beyond stack limit and then popping them all */

  PASS();
}

struct filesystem_counter
{
  size_t numdirs;
  size_t numfiles;
};
srsFILESYSTEM_VISIT_ACTION filesystem_counter_iterator(const char *path, void *userdata)
{
  struct filesystem_counter *counter = ((struct filesystem_counter *)userdata);
  srsLOG_NOTIFY("ITERATOR: VISTITING %s", path);
  if (srsDir_Exists(path))
  {
    counter->numdirs++;
    srsLOG_NOTIFY("ITERATOR: numdirs = %zu", counter->numdirs);
    return srsFILESYSTEM_VISIT_RECURSE;
  }
  else
  {
    counter->numfiles++;
    srsLOG_NOTIFY("ITERATOR: numfiles = %zu", counter->numfiles);
  }
  return srsFILESYSTEM_VISIT_CONTINUE;
}

TEST TestIteration(void)
{
  const char *files[] = {
    "./a/file",
    "./a/b/e/file",
  };
  const char *dirs[] = {
    "./a",
    "./a/b",
    "./a/b/c",
    "./a/b/d",
    "./a/b/e",
    "./a/b/e/f"
  };
  size_t numfiles = sizeof(files) / sizeof(files[0]);
  size_t numdirs = sizeof(dirs) / sizeof(dirs[0]);
  size_t i;
  for (i = 0; i < numdirs; i++)
  {
    ASSERT(srsDir_Exists(dirs[i]) || srsDir_Create(dirs[i]));
  }
  for (i = 0; i < numfiles; i++)
  {
    ASSERT(srsFile_Exists(files[i]) || srsFile_Create(files[i]));
  }

  /* Test bad input */
  srsLOG_NOTIFY("TestIteration: Testing bad input");
  ASSERT_EQ(false, srsFileSystem_Iterate(NULL, NULL, NULL));
  ASSERT_EQ(false, srsFileSystem_Iterate(dirs[0], NULL, NULL));
  ASSERT_EQ(false, srsFileSystem_Iterate(dirs[0], (void *)0xDEADBEEF, NULL));
  ASSERT_EQ(false, srsDir_Exists("not-a-directory-lol"));
  ASSERT_EQ(false, srsFileSystem_Iterate("not-a-directory-lol", (void *)0xDEADBEEF, &filesystem_counter_iterator));

  /* Test counting files and dirs */
  srsLOG_NOTIFY("TestIteration: Testing counter");
  struct filesystem_counter counter = {0};
  ASSERT_EQ(true, srsFileSystem_Iterate(dirs[0], &counter, &filesystem_counter_iterator));
  /* We subtract 1 from numdirs because we start in dirs[0], so that one won't increment the counter */
  ASSERT_EQ_FMT(numfiles, counter.numfiles, "%zu");
  ASSERT_EQ_FMT(numdirs-1, counter.numdirs, "%zu");

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
  RUN_TEST(TestSetGetCWD);
  printf(kiokuSTRING_LF);
  srsDir_SetCWD(TESTDIR);
  RUN_TEST(TestPushPopCWD);
  printf(kiokuSTRING_LF);
  srsDir_SetCWD(TESTDIR);
  RUN_TEST(TestGetFullPath);
  printf(kiokuSTRING_LF);
  srsDir_SetCWD(TESTDIR);
  RUN_TEST(TestIteration);
}
/* Add definitions that need to be in the test runner's main file. */
GREATEST_MAIN_DEFS();
int main(int argc, char **argv)
{
  GREATEST_MAIN_BEGIN();      /* command-line options, initialization. */

  srsLOG_NOTIFY("Max Path Length = %zu", (size_t)srsPATH_MAX);
  RUN_SUITE(test_filesystem);

  GREATEST_MAIN_END();        /* display results */
}

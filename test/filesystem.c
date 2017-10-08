#include <tap.h>

#include "kioku.h"
#include "string.h"
#include <assert.h>

void test_up_path()
{
  printf("Testing kioku_path_up_index...\r\n");
  /* Test full path traversal */
  char path[] = "a/ab/abc/abcd/abcde.txt";
  int32_t index = kioku_path_up_index(path, -1);
  cmp_ok(index, "==", 13);
  path[index] = '\0';
  is(path, "a/ab/abc/abcd");
  index = kioku_path_up_index(path, index);
  cmp_ok(index, "==", 8);
  path[index] = '\0';
  is(path, "a/ab/abc");
  index = kioku_path_up_index(path, index);
  cmp_ok(index, "==", 4);
  path[index] = '\0';
  is(path, "a/ab");
  index = kioku_path_up_index(path, index);
  cmp_ok(index, "==", 1);
  path[index] = '\0';
  is(path, "a");
  index = kioku_path_up_index(path, index);
  cmp_ok(index, "==", -1);

  /* Test some potential edge cases */
  index = kioku_path_up_index(NULL, -1);
  cmp_ok(index, "==", -1);

  index = kioku_path_up_index("", -1);
  cmp_ok(index, "==", -1);

  index = kioku_path_up_index("abcdefg", -1);
  cmp_ok(index, "==", -1);

  index = kioku_path_up_index("/", -1);
  cmp_ok(index, "==", 0);
}

void test_trim_path()
{
  printf("Testing kioku_path_trimpoints...\r\n");
  const char *path;
  uint32_t start;
  uint32_t end;

  path = NULL;
  kioku_path_trimpoints(path, &start, &end);
  cmp_ok(start, "==", 0, "Valid start");
  cmp_ok(end, "==", 0, "Valid end");

  path = "";
  kioku_path_trimpoints(path, &start, &end);
  cmp_ok(start, "==", 0, "Valid start");
  cmp_ok(end, "==", 0, "Valid end");

  path = "/path";
  kioku_path_trimpoints(path, &start, &end);
  cmp_ok(start, "==", 0, "Valid start");
  cmp_ok(end, "==", 4, "Valid end");

  path = "/path/";
  kioku_path_trimpoints(path, &start, &end);
  cmp_ok(start, "==", 0, "Valid start");
  cmp_ok(end, "==", 4, "Valid end");
  ok(strncmp(path + start, "/path", end - start) == 0, "Valid substring");

  path = "///path//";
  kioku_path_trimpoints(path, &start, &end);
  cmp_ok(start, "==", 2, "Valid start");
  cmp_ok(end, "==", start + 4, "Valid end");
  ok(strncmp(path + start, "/path", end - start) == 0, "Valid substring");
}

#define DESTSIZE 16
void test_concat_path()
{
  printf("Testing kioku_path_concat...\r\n");
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
  cmp_ok(strlen(dest), "==", got_len);
  cmp_ok(strlen(expected_path), "==", got_len);
  is(dest, expected_path);
  cmp_ok(got_len, "<", DESTSIZE - 1);
  cmp_ok(strlen(dest), "<", DESTSIZE);

  /* Test empty path1 */
  path1 = "";
  path2 = "path/1";
  expected_path = "path/1";
  got_len = kioku_path_concat(dest, sizeof(dest), path1, path2);
  cmp_ok(strlen(dest), "==", got_len);
  cmp_ok(strlen(expected_path), "==", got_len);
  is(dest, expected_path);
  cmp_ok(got_len, "<", DESTSIZE - 1);
  cmp_ok(strlen(dest), "<", DESTSIZE);

  /* Test empty path2 */
  path1 = "/test";
  path2 = "";
  expected_path = "/test";
  got_len = kioku_path_concat(dest, sizeof(dest), path1, path2);
  cmp_ok(strlen(dest), "==", got_len);
  cmp_ok(strlen(expected_path), "==", got_len);
  is(dest, expected_path);
  cmp_ok(got_len, "<", DESTSIZE - 1);
  cmp_ok(strlen(dest), "<", DESTSIZE);

  /* Test perfect input */
  path1 = "/test/";
  path2 = "path/1";
  expected_path = "/test/path/1";
  got_len = kioku_path_concat(dest, sizeof(dest), path1, path2);
  cmp_ok(strlen(dest), "==", got_len);
  cmp_ok(strlen(expected_path), "==", got_len);
  is(dest, expected_path);
  cmp_ok(got_len, "<", DESTSIZE - 1);
  cmp_ok(strlen(dest), "<", DESTSIZE);

  /* Test removal of redundant slashes */
  path1 = "///test//";
  path2 = "//path/1///";
  got_len = kioku_path_concat(dest, sizeof(dest), path1, path2);
  cmp_ok(strlen(dest), "==", got_len);
  cmp_ok(strlen(expected_path), "==", got_len);
  is(dest, expected_path);
  cmp_ok(got_len, "<", DESTSIZE - 1);
  cmp_ok(strlen(dest), "<", DESTSIZE);

  /* Test adding necessary slashes */
  path1 = "/test";
  path2 = "path/1";
  got_len = kioku_path_concat(dest, sizeof(dest), path1, path2);
  cmp_ok(strlen(dest), "==", got_len);
  cmp_ok(strlen(expected_path), "==", got_len);
  is(dest, expected_path);
  cmp_ok(got_len, "<", DESTSIZE - 1);
  cmp_ok(strlen(dest), "<", DESTSIZE);

  /* Test adding a very small path */
  path1 = "/";
  path2 = "1";
  expected_path = "/1";
  got_len = kioku_path_concat(dest, sizeof(dest), path1, path2);
  cmp_ok(strlen(dest), "==", got_len);
  cmp_ok(strlen(expected_path), "==", got_len);
  is(dest, expected_path);
  cmp_ok(got_len, "<", DESTSIZE - 1);
  cmp_ok(strlen(dest), "<", DESTSIZE);

  /* Test insufficient size */
  path1 = "/test/much/";
  path2 = "/longer/path/1";
  expected_path = "/test/much/longer/path/1";
  got_len = kioku_path_concat(dest, sizeof(dest), path1, path2);
  cmp_ok(strlen(expected_path), "==", got_len);
  isnt(dest, expected_path);
  cmp_ok(strncmp(dest, expected_path, sizeof(dest) - 1), "==", 0);
  cmp_ok(got_len, ">", DESTSIZE - 1);
  cmp_ok(strlen(dest), "==", DESTSIZE - 1);

  /* Test various valid ways of size probing */
  got_len = kioku_path_concat(NULL, sizeof(dest), path1, path2);
  cmp_ok(strlen(expected_path), "==", got_len);
  cmp_ok(got_len, ">", DESTSIZE - 1);
  got_len = kioku_path_concat(dest, 0, path1, path2);
  cmp_ok(strlen(expected_path), "==", got_len);
  cmp_ok(got_len, ">", DESTSIZE - 1);
  got_len = kioku_path_concat(NULL, 0, path1, path2);
  cmp_ok(strlen(expected_path), "==", got_len);
  cmp_ok(got_len, ">", DESTSIZE - 1);

  /* Test dangerous append prevention */
  dest[0] = '.';
  dest[1] = '\0';
  path1 = dest;
  path2 = "dont/append";
  expected_path = "./dont/append";
  got_len = kioku_path_concat(dest, sizeof(dest), path1, path2);
  cmp_ok(got_len, "<", 0);
  isnt(dest, expected_path);
  cmp_ok(got_len, "<", DESTSIZE - 1);
  cmp_ok(strlen(dest), "<", DESTSIZE);
}

void test_file_manage() {
  printf("Testing file management functions...\r\n");
  const char *path = "a/b/c/a.txt";
  ok(kioku_filesystem_exists(path) == false);
  ok(kioku_filesystem_create(path));
  ok(kioku_filesystem_exists(path));
  ok(kioku_filesystem_create(path) == false);

  ok(kioku_filesystem_delete(path));
  ok(kioku_filesystem_exists(path) == false);
  ok(kioku_filesystem_delete(path) == false);

  ok(kioku_filesystem_exists("a"));
  ok(kioku_filesystem_delete("a") == false);
  ok(kioku_filesystem_exists("a"));

  ok(kioku_filesystem_delete("a/b") == false);
  ok(kioku_filesystem_exists("a/b"));

  ok(kioku_filesystem_delete("a/b/c"));
  ok(kioku_filesystem_exists("a/b/c") == false);
  ok(kioku_filesystem_delete("a/b/c") == false);

  ok(kioku_filesystem_delete("a/b"));
  ok(kioku_filesystem_exists("a/b") == false);
  ok(kioku_filesystem_delete("a/b") == false);

  ok(kioku_filesystem_delete("a"));
  ok(kioku_filesystem_exists("a") == false);
  ok(kioku_filesystem_delete("a") == false);

  /* Test a file-only rename */
  ok(kioku_filesystem_create(path));
  const char *newpath = "a/b/c/d.txt";
  ok(kioku_filesystem_exists(newpath) == false);
  ok(kioku_filesystem_rename(path, newpath));
  ok(kioku_filesystem_exists(path) == false);
  ok(kioku_filesystem_exists(newpath));

  /* Test a directory-only rename */
  path = "a/b/c";
  newpath = "a/b/d";
  ok(kioku_filesystem_exists(path));
  ok(kioku_filesystem_exists(newpath) == false);
  ok(kioku_filesystem_rename(path, newpath));
  ok(kioku_filesystem_exists(path) == false);
  ok(kioku_filesystem_exists(newpath));

  /* Test a move-like rename */
  path = "a/b/d/d.txt";
  newpath = "a/b/c/d.txt";
  ok(kioku_filesystem_exists(path));
  ok(kioku_filesystem_exists(newpath) == false);
  ok(kioku_filesystem_rename(path, newpath) == false);
  ok(kioku_filesystem_exists(path));
  ok(kioku_filesystem_exists(newpath) == false);

  /* Cleanup */
  ok(kioku_filesystem_delete(path));
  ok(kioku_filesystem_delete("a/b/d"));
  ok(kioku_filesystem_delete("a/b"));
  ok(kioku_filesystem_delete("a"));

  char fullpath[255] = {0};
  kioku_path_concat(fullpath, sizeof(fullpath), BUILDDIR, "a/b/c/a.txt");
  ok(kioku_filesystem_exists(fullpath) == false);
  ok(kioku_filesystem_create(fullpath));
  ok(kioku_filesystem_exists(fullpath));
  ok(kioku_filesystem_create(fullpath) == false);
  ok(kioku_filesystem_delete(fullpath));
  ok(kioku_filesystem_exists(fullpath) == false);

  kioku_path_concat(fullpath, sizeof(fullpath), BUILDDIR, "a/b/c");
  ok(kioku_filesystem_delete(fullpath));
  ok(kioku_filesystem_exists(fullpath) == false);
  ok(kioku_filesystem_delete(fullpath) == false);

  kioku_path_concat(fullpath, sizeof(fullpath), BUILDDIR, "a/b");
  printf("%s\r\n", fullpath );
  ok(kioku_filesystem_delete(fullpath));
  ok(kioku_filesystem_exists(fullpath) == false);
  ok(kioku_filesystem_delete(fullpath) == false);

  kioku_path_concat(fullpath, sizeof(fullpath), BUILDDIR, "a");
  printf("%s\r\n", fullpath );
  ok(kioku_filesystem_delete(fullpath));
  ok(kioku_filesystem_exists(fullpath) == false);
  ok(kioku_filesystem_delete(fullpath) == false);
}
int main(int argc, char **argv)
{
  plan(NO_PLAN);
/* kioku_filesystem_init(); */
  test_up_path();
  test_trim_path();
  test_concat_path();
  test_file_manage();
/* kioku_filesystem_exit(); */
  done_testing();
}

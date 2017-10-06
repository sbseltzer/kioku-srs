#include <tap.h>

#include "kioku.h"
#include "string.h"

void test_trim_path()
{
  const char *path;
  uint32_t start;
  uint32_t end;

  path = NULL;
  kioku_path_trimpoints(path, &start, &end);
  ok(start == 0);
  ok(end == 0);

  path = "";
  kioku_path_trimpoints(path, &start, &end);
  ok(start == 0);
  ok(end == 0);

  path = "/path";
  kioku_path_trimpoints(path, &start, &end);
  ok(start == 0);
  ok(end == 4);

  path = "/path/";
  kioku_path_trimpoints(path, &start, &end);
  cmp_ok(start, "==", 0);
  cmp_ok(end, "==", 4);
  ok(strncmp(path + start, "/path", end - start) == 0);

  path = "///path//";
  kioku_path_trimpoints(path, &start, &end);
  cmp_ok(start, "==", 2);
  cmp_ok(end, "==", start + 4);
  ok(strncmp(path + start, "/path", end - start) == 0);
}

void test_concat_path()
{
  const char *path1;
  const char *path2;
  const char *expected_path;
  int32_t got_len;
  char dest[16] = {0};

  /* Test perfect input */
  path1 = "/test/";
  path2 = "path/1";
  expected_path = "/test/path/1";
  got_len = kioku_path_concat(dest, sizeof(dest), path1, path2);
  cmp_ok(strlen(dest), "==", got_len);
  cmp_ok(strlen(expected_path), "==", got_len);
  is(dest, expected_path);

  /* Test removal of redundant slashes */
  path1 = "///test//";
  path2 = "//path/1///";
  got_len = kioku_path_concat(dest, sizeof(dest), path1, path2);
  cmp_ok(strlen(dest), "==", got_len);
  cmp_ok(strlen(expected_path), "==", got_len);
  is(dest, expected_path);

  /* Test adding necessary slashes */
  path1 = "/test";
  path2 = "path/1";
  got_len = kioku_path_concat(dest, sizeof(dest), path1, path2);
  cmp_ok(strlen(dest), "==", got_len);
  cmp_ok(strlen(expected_path), "==", got_len);
  is(dest, expected_path);

  /* Test insufficient size */
  path1 = "/test/much/";
  path2 = "/longer/path/1";
  expected_path = "/test/much/longer/path/1";
  cmp_ok(kioku_path_concat(dest, sizeof(dest), path1, path2), "<", 0);
  isnt(dest, expected_path);

  /* Test dangerous append prevention */
  dest[0] = '.';
  dest[1] = '\0';
  path1 = dest;
  path2 = "dont/append";
  expected_path = "./dont/append";
  cmp_ok(kioku_path_concat(dest, sizeof(dest), path1, path2), "<", 0);
  isnt(dest, expected_path);
}

void test_file_manage()
{
  const char *path = "a/b/c/a.txt";
  ok(kioku_filesystem_exists(path) == false);
  ok(kioku_filesystem_create(path));
  ok(kioku_filesystem_exists(path));
  ok(kioku_filesystem_delete(path));
  ok(kioku_filesystem_exists(path) == false);
  ok(kioku_filesystem_exists("a/"));
  ok(kioku_filesystem_delete("a/"));
  ok(kioku_filesystem_exists("a/") == false);
}
int main(int argc, char **argv)
{
/* kioku_filesystem_init(); */

  test_trim_path();
  test_concat_path();
  test_file_manage();
/* kioku_filesystem_exit(); */
}

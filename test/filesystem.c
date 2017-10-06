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

void test_concat_path()
{
  const char *path1;
  const char *path2;
  const char *expected_path;
  int32_t got_len;
  char dest[16] = {0};

  /* Test empty paths */
  path1 = "";
  path2 = "";
  expected_path = "";
  got_len = kioku_path_concat(dest, sizeof(dest), path1, path2);
  cmp_ok(strlen(dest), "==", got_len);
  cmp_ok(strlen(expected_path), "==", got_len);
  is(dest, expected_path);

  /* Test empty path1 */
  path1 = "";
  path2 = "path/1";
  expected_path = "path/1";
  got_len = kioku_path_concat(dest, sizeof(dest), path1, path2);
  cmp_ok(strlen(dest), "==", got_len);
  cmp_ok(strlen(expected_path), "==", got_len);
  is(dest, expected_path);

  /* Test empty path2 */
  path1 = "/test";
  path2 = "";
  expected_path = "/test";
  got_len = kioku_path_concat(dest, sizeof(dest), path1, path2);
  cmp_ok(strlen(dest), "==", got_len);
  cmp_ok(strlen(expected_path), "==", got_len);
  is(dest, expected_path);

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

  /* Test adding a very small path */
  path1 = "/";
  path2 = "1";
  expected_path = "/1";
  got_len = kioku_path_concat(dest, sizeof(dest), path1, path2);
  cmp_ok(strlen(dest), "==", got_len);
  cmp_ok(strlen(expected_path), "==", got_len);
  is(dest, expected_path);

  /* Test insufficient size */
  path1 = "/test/much/";
  path2 = "/longer/path/1";
  expected_path = "/test/much/longer/path/1";
  cmp_ok(kioku_path_concat(dest, sizeof(dest), path1, path2), "==", sizeof(dest));
  isnt(dest, expected_path);
  cmp_ok(strncmp(dest, expected_path, sizeof(dest)), "==", 0);

  /* Test dangerous append prevention */
  dest[0] = '.';
  dest[1] = '\0';
  path1 = dest;
  path2 = "dont/append";
  expected_path = "./dont/append";
  cmp_ok(kioku_path_concat(dest, sizeof(dest), path1, path2), "<", 0);
  isnt(dest, expected_path);
}

void test_file_manage() {
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
/* kioku_filesystem_init(); */
  test_trim_path();
  test_concat_path();
  test_file_manage();
/* kioku_filesystem_exit(); */
}

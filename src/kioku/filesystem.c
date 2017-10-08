#include "kioku/filesystem.h"
#include "kioku/log.h"
#include "tinydir.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <direct.h>
#include <io.h>
/* Microsoft (correctly) refuses to recognize strdup as being ISO C compliant,
   despite being part of POSIX, and recommends its own standard-compliant name. */
#define strdup _strdup
#else
#include <unistd.h>
#endif

void kioku_path_trimpoints(const char *path, uint32_t *start, uint32_t *end)
{
  uint32_t path_start = 0;
  uint32_t path_end = 0;
  if (start == NULL || end == NULL)
  {
    return;
  }
  if (path != NULL)
  {
    path_end = strlen(path);
    /* Index of strlen is at the null terminator, but we want to end at the last valid char. */
    if (path_end > 0)
    {
      path_end--;
    }
    /* Strip extra slashes on end of path */
    while ((path_end > path_start) && (path[path_end] == '/'))
    {
      path_end--;
    }
    /* Strip extra slashes at start of path */
    while ((path_start < path_end) && (path[path_start] == '/'))
    {
      path_start++;
    }
    /* If we stripped ALL slashes, make sure there's one leading on left */
    if (path_start > 0)
    {
      path_start--;
    }
  }
  *start = path_start;
  *end = path_end;
}

int32_t kioku_path_concat(char *dest, size_t destsize, const char *path1, const char *path2)
{
  /* Don't let the user use this as an append or anything dangerous like that. */
  if (dest == path1 || dest == path2)
  {
    return -1;
  }
  uint32_t path1_start, path1_end, path2_start, path2_end;
  kioku_path_trimpoints(path1, &path1_start, &path1_end);
  kioku_path_trimpoints(path2, &path2_start, &path2_end);
  /* The right hand portion should not have a leading slash */
  if (path2[path2_start] == '/')
  {
    path2_start++;
  }
  uint32_t outlen = 0;
  uint32_t pi;
  size_t max_index = destsize - 1;
  for (pi = path1_start; (outlen < max_index) && (pi <= path1_end) && (path1[pi] != '\0'); pi++)
  {
    dest[outlen] = path1[pi];
    outlen++;
  }
  if ((outlen > 0) && (outlen < max_index) && (dest[outlen-1] != '/') && (path2[path2_start] != '\0'))
  {
    dest[outlen] = '/';
    outlen++;
  }
  for (pi = path2_start; (outlen < max_index) && (pi <= path2_end) && (path2[pi] != 0); pi++)
  {
    dest[outlen] = path2[pi];
    outlen++;
  }
  /* Sanity check */
  assert(outlen <= max_index);
  /* Null terminate */
  dest[outlen] = '\0';
  assert(outlen == strlen(dest));
  return outlen;
}

int32_t kioku_path_up_index(const char *path, int32_t start_index)
{
  int32_t result = -1;
  if (path == NULL)
  {
    return result;
  }
  /* March backward til it hits a separator. */
  for (result = strlen(path) - 1; (result >= 0) && (path[result] != '/'); result--);
  return result;
}

bool kioku_filesystem_create(const char *path)
{
  if (kioku_filesystem_exists(path))
  {
    return false;
  }
  /* Do not allow creating an empty directory */
  if (path[strlen(path)] == '/')
  {
    return false;
  }
  char *dupedpath = strdup(path);
  char *found = dupedpath;
  while (found != NULL)
  {
    found = strchr(found, '/');
    if (found == NULL)
    {
      break;
    }
    if (kioku_filesystem_exists(dupedpath))
    {
      continue;
    }
    char oldval = found[1];
    found[1] = 0;
#ifdef _WIN32
    bool ok = _mkdir(dupedpath) == 0;
#else
    bool ok = mkdir(dupedpath, 0700) == 0;
#endif
    /* We don't do anything with ok for now. If we break the loop here we won't get to attempt all the directories we need to. */
    found[1] = oldval;
    found++;
  }
  free(dupedpath);
  /* Creating file */
#ifdef _WIN32
  FILE *fp = NULL;
  fopen_s(&fp, path, "w");
#else
  FILE *fp = fopen(path, "w");
#endif
  return (fp != NULL) && (fclose(fp) == 0);
}

bool kioku_filesystem_rename(const char *path, const char *newpath)
{
  return rename(path, newpath) == 0;
}

#define KIOKU_DIR_MAX_DEPTH 128
bool kioku_filesystem_delete(const char *path)
{
  bool result = false;
  if (!kioku_filesystem_exists(path))
  {
    return result;
  }
#ifdef _WIN32
  /* https://stackoverflow.com/a/6218957 */
  DWORD dwAttrib = GetFileAttributes(path);
  if (dwAttrib & FILE_ATTRIBUTE_DIRECTORY)
  {
    result = _rmdir(path) == 0;
  }
  else
  {
    result = remove(path) == 0;
  }
#else
  result = remove(path) == 0;
#endif
  return result;
}

bool kioku_filesystem_exists(const char *path)
{
#ifdef _WIN32
  /* https://stackoverflow.com/a/6218957 */
  return GetFileAttributes(path) != INVALID_FILE_ATTRIBUTES;
#else
  return access(path, R_OK) == 0;
#endif
}

#include "kioku/filesystem.h"
#include "kioku/log.h"
#include "tinydir.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <direct.h>
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
  /* Don't let the user use this as an append. C99 states that using sprintf in this way could invoke undefined behaviour. */
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
  for (pi = path1_start; (outlen < destsize) && (pi <= path1_end) && (path1[pi] != '\0'); pi++)
  {
    dest[outlen] = path1[pi];
    outlen++;
  }
  if ((outlen > 0) && (dest[outlen-1] != '/') && (path2[path2_start] != '\0'))
  {
    dest[outlen] = '/';
    outlen++;
  }
  for (pi = path2_start; (outlen < destsize) && (pi <= path2_end) && (path2[pi] != 0); pi++)
  {
    dest[outlen] = path2[pi];
    outlen++;
  }
  /* Null terminate */
  dest[outlen] = '\0';
  /* Sanity check */
  assert(outlen <= destsize);
  assert(outlen == strlen(dest));
  return outlen;
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
  FILE *fp = fopen(path, "w");
  return (fp != NULL) && (fclose(fp) == 0);
}

bool kioku_filesystem_rename(const char *path, const char *newpath)
{
  return rename(path, newpath) == 0;
}

#define KIOKU_DIR_MAX_DEPTH 128
bool kioku_filesystem_delete(const char *path)
{
  if (!kioku_filesystem_exists(path))
  {
    return false;
  }
  return remove(path) == 0;
}

bool kioku_filesystem_exists(const char *path)
{
#ifdef _WIN32
  return access(path, 00) == 0;
#else
  return access(path, R_OK) == 0;
#endif
}

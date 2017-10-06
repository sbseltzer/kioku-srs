#include "kioku/filesystem.h"
#include "kioku/log.h"
#include <assert.h>

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
    if (path_end > 0)
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
    /* Strip extra slashes on end of path */
    while ((path_end > path_start) && (path[path_end] == '/'))
    {
      path_end--;
    }
  }
  *start = path_start;
  *end = path_end;
}

int32_t kioku_path_concat(char *dest, size_t destsize, const char *path1, const char *path2)
{
  /* Don't let the user use this as an append. C99 states that using sprintf in this way could invoke undefined behaviour. */
  if (dest == path1)
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
  uint32_t path1_len = (path1_end - path1_start) + 1;
  uint32_t path2_len = (path2_end - path2_start) + 1;
  /* First see if there's enough room */
  if ((path1_len + path2_len + strlen(KIOKU_DIRSEP) + 1) > destsize)
  {
    return -1;
  }
  uint32_t nbytes = 0;
  /* Tack on trimmed path1 */
  strncpy(dest, path1 + path1_start, path1_len);
  nbytes += path1_len;
  /* Tack on dir separator */
  nbytes += sprintf(dest + nbytes, "%s", KIOKU_DIRSEP);
  /* Tack on trimmed path2 */
  strncpy(dest + nbytes, path2 + path2_start, path2_len);
  nbytes += path2_len;
  /* Null terminate */
  dest[nbytes] = '\0';
  /* Sanity check */
  assert(nbytes < destsize);
  assert(nbytes == strlen(dest));
  return nbytes;
}

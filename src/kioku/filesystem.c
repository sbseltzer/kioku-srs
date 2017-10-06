#include "kioku/filesystem.h"

int32_t kioku_path_concat(char *dest, size_t destsize, const char *path1, const char *path2)
{
  /* Don't let the user use this as an append. C99 states that using sprintf in this way could invoke undefined behaviour. */
  if (dest == path1)
  {
    return -1;
  }
  /* Since snprintf is not available everywhere, we first check if there's enough space, then use the less-safe sprintf. */
  uint32_t path1_start = 0;
  uint32_t path1_end = strlen(path1);
  uint32_t path2_start = 0;
  uint32_t path2_end = strlen(path2);
  /* Strip extra slashes at start of path1 */
  while ((path1_start < path1_end) && (path1[path1_start] == '/'))
  {
    path1_start++;
  }
  /* If we stripped ALL slashes, make sure there's one leading on left */
  if (path1_start > 0)
  {
    path1_start--;
  }
  /* Strip extra slashes on end of path1 */
  while ((path1_end > path1_start) && (path1[path1_end] == '/'))
  {
    path1_end--;
  }
  /* Strip extra slashes at start of path2 */
  while ((path2_start < path2_end) && (path2[path2_start] == '/'))
  {
    path2_start++;
  }
  /* Strip extra slashes on end of path2 */
  while ((path2_len > path2_start) && (path2[path2_len] == '/'))
  {
    path2_len--;
  }
  if (((path1_end - path1_start) + (path2_end - path2_start) + 1) < destsize)
  {
    return -1;
  }
  return sprintf(dest, "%s" KIOKU_DIRSEP "%s", path1, path2);
}

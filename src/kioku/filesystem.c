#include "kioku/filesystem.h"

ssize_t kioku_path_concat(char *dest, size_t destsize, const char *path1, const char *path2)
{
  /* Don't let the user use this as an append. C99 states that using sprintf in this way could invoke undefined behaviour. */
  if (dest == path1)
  {
    return -1;
  }
  /* Since snprintf is not available everywhere, we first check if there's enough space, then use the less-safe sprintf. */
  if ((strlen(path1) + strlen(path2) + 1) < destsize)
  {
    return -1;
  }
  return sprintf(dest, "%s" KIOKU_DIRSEP "%s", path1, path2);
}

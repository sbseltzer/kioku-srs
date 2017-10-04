#include "kioku/log.h"
#include <errno.h> /* errno */
#include <string.h> /* strerror */

/**
 * Used internally to get the logfile handle singleton
 */
FILE *kLOG_GetHandle()
{
  static FILE *handle = NULL;
  errno = 0;
  handle = (handle != NULL) ? handle : fopen(KIOKU_LOGFILE, "a");
  int32_t errno_captured = errno;
  if (handle == NULL)
  {
    fprintf(stderr, "Failed to get logfile handle: %s\r\n", strerror(errno_captured));
  }
  return handle;
}
/**
 * Call at end of execution.
 */
void kLOG_Exit()
{
  FILE *handle = kLOG_GetHandle();
  if (handle != NULL)
  {
    errno = 0;
    fclose(handle);
    int32_t errno_captured = errno;
    if (errno_captured != 0)
    {
      fprintf(stderr, "Failed to close logfile handle: %s\r\n", strerror(errno_captured));
    }
  }
}


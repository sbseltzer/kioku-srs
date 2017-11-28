#include "kioku/log.h"
#include "kioku/string.h"
#include <errno.h> /* errno */
#include <string.h> /* strerror, strstr */

/**
 * Used internally to get the logfile handle singleton
 */
FILE *srsLog_GetHandle()
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

/* va_copy support for MSVC - https://stackoverflow.com/a/28641350 */
#if defined _MSVC && _MSC_VER < 1800 /* va_copy is available in vc2013 and onwards */
#define va_copy(a, b) (a = b)
#endif

static int32_t srsLog_GetLineCount()
{
  static int32_t linecount = -1;
  if (linecount == -1)
  {
    linecount = 0;
    int c = 0;
    do
    {
      c = fgetc(srsLOGFILE);
      if (c == '\n')
      {
        linecount++;
      }
    } while (c != EOF);
  }
  return linecount;
}

bool srsLog_VWriteToStream(FILE *stream, const char *_FILENAME, uint32_t _LINENUMBER, const char *_FUNCNAME, const char *format, va_list args)
{
  const char *filepath = NULL;

  filepath = srsString_GetSourcePath(_FILENAME);

  /* Print the log prefix that indicates where it comes from. */
  /** TODO Reimplement it to write the whole log entry to a buffer first so we can use write to print it atomically */
  fprintf(stream, "%s:%u (%s): ", filepath, _LINENUMBER, _FUNCNAME);

  /* Use the copied args */
  vfprintf(stream, format, args);

  /* Newline */
  fprintf(stream, "\r\n");

  /* Flush! If this logged a critical error we don't want to risk accidentally missing it! */
  fflush(stream);

  return true;
}

bool srsLog_WriteToStream(FILE *stream, const char *_FILENAME, uint32_t _LINENUMBER, const char *_FUNCNAME, const char *format, ...)
{
  bool ok = false;
  va_list args;
  va_start(args, format);
  ok = srsLog_VWriteToStream(stream, _FILENAME, _LINENUMBER, _FUNCNAME, format, args);
  va_end(args);
  return ok;
}

int32_t srsLog_WriteToStreamAndLog(FILE *stream, const char *_FILENAME, uint32_t _LINENUMBER, const char *_FUNCNAME, const char *format, ...)
{
  bool ok = false;
  int32_t line_number = -1;
  va_list args;
  va_list args_copy;
  va_start(args, format);
  /* We don't actually care about whether this one succeeds, though it'd be nice if it did */
  va_copy(args_copy, args);
  ok = srsLog_VWriteToStream(stream, _FILENAME, _LINENUMBER, _FUNCNAME, format, args);
  va_end(args_copy);
  if (!ok)
  {
    fprintf(stderr, "%s:%u: Failed to write to stream (fd = %d)!", srsString_GetSourcePath(_FILENAME), _LINENUMBER, fileno(stream));
  }
  /* If we didn't explicitly tell it to write to the logfile, write to it in addition to whatever stream we just wrote to. */
  if (stream != srsLOGFILE)
  {
    va_copy(args_copy, args);
    ok = srsLog_VWriteToStream(srsLOGFILE, _FILENAME, _LINENUMBER, _FUNCNAME, format, args);
    va_end(args_copy);
  }
  /* Record the line number that was written to the logfile */
  line_number = srsLog_GetLineCount();
  return line_number;
}

/**
 * Call at end of execution.
 */
void srsLog_Exit()
{
  FILE *handle = srsLog_GetHandle();
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


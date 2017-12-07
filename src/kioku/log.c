#include "kioku/log.h"
#include "kioku/debug.h"
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

static const char *srsLog_SEARCH_PATH_SEGMENTS[] = {
  "src/",
  "src\\",
  "include/",
  "include\\",
  "test/",
  "test\\"
};

/* Find the start of the *meaningful* part of __FILE__ since it will bake the full paths the host it was compiled on into the binary. */
/* Come to think of it, that means your data section size could depend on how deep into the filesystem your project was when it was compiled. Freaky. */
/* BUG: if none of the search path segments are found, this screws up _FUNCNAME in my vprintf-like functions */
const char *srsLog_GetSourcePath(const char *_FILENAME)
{
  const char *filepath = _FILENAME;
  size_t i;
  size_t n = sizeof(srsLog_SEARCH_PATH_SEGMENTS) / sizeof(srsLog_SEARCH_PATH_SEGMENTS[0]);
  if (_FILENAME == NULL)
  {
    return NULL;
  }
  for (i = 0; i < n; i++)
  {
    char *subpath = strstr(_FILENAME, srsLog_SEARCH_PATH_SEGMENTS[i]);
    srsASSERT(subpath == NULL || _FILENAME <= subpath);
    if (subpath != NULL)
    {
      filepath = subpath;
      break;
    }
  }
  return filepath;
}

bool srsLog_VWriteToStream(FILE *stream, const char *_FILENAME, uint32_t _LINENUMBER, const char *_FUNCNAME, const char *format, va_list args)
{
  /* Print the log prefix that indicates where it comes from. */
  /** TODO Reimplement it to write the whole log entry to a buffer first so we can use write to print it atomically */
  fprintf(stream, "%s:%u (%s): ", _FILENAME, _LINENUMBER, _FUNCNAME);

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
  va_start(args, format);
  /* We don't actually care about whether this one succeeds, though it'd be nice if it did */
  ok = srsLog_VWriteToStream(stream, _FILENAME, _LINENUMBER, _FUNCNAME, format, args);
  va_end(args);
  if (!ok)
  {
    fprintf(stderr, "%s:%u: Failed to write to stream (fd = %d)!", _FILENAME, _LINENUMBER, fileno(stream));
  }
  /* If we didn't explicitly tell it to write to the logfile, write to it in addition to whatever stream we just wrote to. */
  if (stream != srsLOGFILE)
  {
    va_start(args, format);
    ok = srsLog_VWriteToStream(srsLOGFILE, _FILENAME, _LINENUMBER, _FUNCNAME, format, args);
    va_end(args);
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


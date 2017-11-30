#include "kioku/error.h"
#include "kioku/log.h"
#include "kioku/debug.h"
#include <string.h>
#include <stdlib.h>
#include <stddef.h>

static char srsError_Name[srsERROR_NAME_SIZE]       = {0};
static char srsError_Message[srsERROR_MESSAGE_SIZE] = {0};

srsTHREADLOCAL static srsERROR_DATA srsError_Last = {
  srsOK,                        /* Result code */
  srsError_Name,                /* Value never changes, contents do */
  srsError_Message,             /* Value never changes, contents do */
  NULL,                         /* File name */
  NULL,                         /* Function name */
  0,                            /* Line number */
  0,                            /* Log line number */
  0                             /* Captured errno */
};

/** Returns a copy of global thread-local error data struct */
srsERROR_DATA srsError_Get()
{
  return srsError_Last;
}

/** Resets the global thread-local error data struct */
void srsError_Reset()
{
  /* Reset to defaults */
  srsError_Set(srsOK,           /* Result code */
               "OK",            /* Name */
               "",              /* Message */
               0,               /* Captured errno */
               "N/A",           /* File name */
               0,               /* Line number */
               "N/A"            /* Function name */
    );
}

#define srsABS(x) ((x) * ((x) < 0 ? -1 : 1))

/** Sets up the global thread local error data struct */
void srsError_Set(srsRESULT code, const char *name, const char *message, int32_t errno_capture,
                           const char *_FILENAME, int32_t _LINENUMBER, const char *_FUNCNAME)
{
  ptrdiff_t offset = 0;

  /* Violating the following could invoke undefined behaviour */
  offset = srsABS(name - srsError_Name);
  srsASSERT(offset >= srsERROR_NAME_SIZE);
  offset = srsABS(message - srsError_Message);
  srsASSERT(offset >= srsERROR_MESSAGE_SIZE);
  /* Setup error struct */
  srsError_Last.code = code;
  strncpy(srsError_Name, name, sizeof(srsError_Name));
  srsError_Name[srsERROR_NAME_SIZE-1] = '\0';
  strncpy(srsError_Message, message, sizeof(srsError_Message));
  srsError_Message[srsERROR_MESSAGE_SIZE-1] = '\0';
  srsError_Last.captured_errno = errno_capture;
  srsError_Last.file_name = _FILENAME;
  srsError_Last.line_number = _LINENUMBER;
  srsError_Last.function_name = _FUNCNAME;
  srsError_SetLogLineNumber(0);
}

/** Specifies a log line number if any */
void srsError_SetLogLineNumber(int32_t log_line_number)
{
  srsError_Last.log_line_number = log_line_number;
}

void srsError_Log()
{
  srsERROR_DATA data = srsError_Get();
  if (data.code != srsOK)
  {
    int32_t line = srsLog_WriteToStreamAndLog(
      stderr,
      data.file_name,
      data.line_number,
      data.function_name,
      "ERROR | Result #%d (%s): %s"kiokuSTRING_LF,
      data.code,
      data.name,
      data.message);
    /** TODO make the srsLOG_* functions more robust and wrappable, and have them return an integer representing the logged line number */
    srsError_SetLogLineNumber(line);
  }
}

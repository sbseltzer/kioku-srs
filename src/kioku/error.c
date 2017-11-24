#include "kioku/error.h"
#include <string.h>
#include <stdlib.h>

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
  NULL,                         /* Dynamic userdata */
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

/** Sets up the global thread local error data struct */
void srsError_Set(srsRESULT code, const char *name, const char *message, int32_t errno_capture,
                           const char *_FILENAME, int32_t _LINENUMBER, const char *_FUNCNAME)
{
  srsError_Last.code = code;
  strncpy(srsError_Name, name, sizeof(srsError_Name));
  strncpy(srsError_Message, message, sizeof(srsError_Message));
  srsError_Last.captured_errno = errno_capture;
  srsError_Last.file_name = _FILENAME;
  srsError_Last.line_number = _LINENUMBER;
  srsError_Last.function_name = _FUNCNAME;
  srsError_SetLogLineNumber(0);
  srsError_SetUserData(NULL);
}

/** Specifies a log line number if any */
void srsError_SetLogLineNumber(int32_t log_line_number)
{
  srsError_Last.log_line_number = log_line_number;
}

/** Specifies some user data associated with the error data struct */
void srsError_SetUserData(void *userdata)
{
  free(srsError_Last.dynamic_userdata);
  srsError_Last.dynamic_userdata = userdata;
}

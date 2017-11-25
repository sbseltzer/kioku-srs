#ifndef _KIOKU_ERROR_H
#define _KIOKU_ERROR_H

#include "kioku/result.h"

#define srsERROR_NAME_SIZE    128
#define srsERROR_MESSAGE_SIZE 1024

/* A fairly heavyweight error struct to communicate as much as possible */
typedef struct _srsERROR_DATA_s
{
  /* Basic information */
  srsRESULT   code;             /* The result code associated with the error */
  const char *name;             /* The error name */
  const char *message;          /* A more detailed error description */
  /* The following have to do with where the error originated from */
  const char *file_name;        /* Origin file */
  const char *function_name;    /* Origin function */
  int32_t     line_number;      /* Origin line */
  /* Extras */
  int32_t     log_line_number;  /* The line in the log file that was created when this error occurred. -1 if no associated log was printed. */
  int32_t     captured_errno;   /* A captured errno */
} srsERROR_DATA;

/**
 * Get the latest error struct state. Similar behaviour to errno.
 * If you haven't previously used @ref srsError_Reset, this could be left over from a previous call.
 * @return A copy of global thread-local error data struct
 */
kiokuAPI srsERROR_DATA srsError_Get();

/**
 * Resets the global thread-local error data struct.
 * Always use this before @ref srsError_Get or you may get a previous struct state.
 */
kiokuAPI void srsError_Reset();

/**
 * Sets up the global thread local error data struct
 * @param[in] code The unique error code. @ref srsOK means no error.
 * @param[in] name A string with a nicer name that's associated with the code. This is copied into a static buffer. If there's not a enough room, it will be truncated.
 * @param[in] message A string with a more detailed description of what happened. This is copied into a static buffer. If there's not a enough room, it will be truncated.
 * @param[in] errno_capture Intended to be a captured errno. Proper use of errno is to capture it in a temp variable at the first available opportunity. That is meant to go here.
 * @param[in] _FILENAME Pass in the __FILE__ macro.
 * @param[in] _LINENUMBER Pass in the __LINE__ macro.
 * @param[in] _FUNCNAME Pass in @ref srsFUNCTION_NAME.
 */
kiokuAPI void srsError_Set(srsRESULT code, const char *name, const char *message, int32_t errno_capture,
                           const char *_FILENAME, int32_t _LINENUMBER, const char *_FUNCNAME);

/**
 * Specifies a log line number associated with the error if any.
 * This is useful to identify where a developer can expect to look for more info such as events leading up to the error.
 * @param[in] log_line_number The line number.
 */
kiokuAPI void srsError_SetLogLineNumber(int32_t log_line_number);

/**
 * More user friendly macro for @ref srsError_Set.
 * @param[in] code Same as code for @ref srsError_Set.
 * @param[in] name_getter A function that returns a string for the name. The return value is intended to be either statically allocated, or interned.
 * @param[in] msg Same as message for @ref srsError_Set.
 * @param[in] errno_capture Same as errno_capture for @ref srsError_Set.
 */
#define srsERROR_SET(code, name_getter, msg, errno_capture)     \
  do {                                                          \
    srsError_Reset();                                           \
    const char *error_code_name = NULL;                         \
    if (name_getter != NULL)                                    \
    {                                                           \
      error_code_name = name_getter(code);                      \
    }                                                           \
    if (error_code_name == NULL)                                \
    {                                                           \
      error_code_name = "UNKNOWN";                              \
    }                                                           \
    srsError_Set(code, error_code_name, message, errno_capture, \
                 __FILE__, __LINE__, srsFUNCTION_NAME);         \
  } while(0)

/**
 * This will take the last set error, create a log for it, then retroactively set its log line number.
 */
#define srsERROR_LOG()                                                  \
  do {                                                                  \
    srsERROR_DATA data = srsError_Get();                                \
    srsLOG_ERROR("%s:%d[%s] | Error #%d (%s): %s",                      \
                 data.file_name, data.line_number, data.function_name,  \
                 data.code, data.name, data.message);                   \
    /** TODO make the srsLOG_* functions more robust and wrappable, and have them return an integer representing the logged line number */ \
    srsError_SetLogLineNumber(0);                                       \
  } while(0)

#endif /* _KIOKU_ERROR_H */

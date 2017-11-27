/**
 * @addtogroup Log
 *
 * A basic logger
 *
 * @{
 */
#ifndef _KIOKU_LOG_H
#define _KIOKU_LOG_H

#ifndef KIOKU_LOGFILE
#define KIOKU_LOGFILE "LOG.txt"
#endif

#include "kioku/decl.h"
#include "kioku/types.h"
#include <stdio.h> /* FILE, vfprintf, fopen, fclose */
#include <stdarg.h> /* va_list */

kiokuAPI FILE *srsLog_GetHandle();

kiokuAPI void srsLog_Exit();

kiokuAPI bool srsLog_VWriteToStream(FILE *stream, const char *_FILENAME, uint32_t _LINENUMBER, const char *_FUNCNAME, const char *format, va_list args);

kiokuAPI bool srsLog_WriteToStream(FILE *stream, const char *_FILENAME, uint32_t _LINENUMBER, const char *_FUNCNAME, const char *format, ...);

kiokuAPI int32_t srsLog_WriteToStreamAndLog(FILE *stream, const char *_FILENAME, uint32_t _LINENUMBER, const char *_FUNCNAME, const char *format, ...);

#define srsLOGFILE srsLog_GetHandle()

#define srsLOG_WRITE(stream, ...)                               \
  srsLog_WriteToStreamAndLog(stream, __FILE__, __LINE__, srsFUNCTION_NAME, __VA_ARGS__);

#define srsLOG_PRINT(...) srsLOG_WRITE(stdout, __VA_ARGS__)
#define srsLOG_ERROR(...) srsLOG_WRITE(stderr, __VA_ARGS__)

#define kLOG_WRITE(...)                                         \
  do {                                                          \
    fprintf(srsLog_GetHandle(), "%s:%d: ", __FILE__, __LINE__); \
    fprintf(srsLog_GetHandle(), __VA_ARGS__);                   \
    fprintf(srsLog_GetHandle(), "\r\n");                        \
    fflush(srsLog_GetHandle());                                 \
  } while (0)

#define srsLOG_NOTIFY(...)                            \
    do {                                              \
      fprintf(stdout, "%s:%d: ", __FILE__, __LINE__); \
      fprintf(stdout, __VA_ARGS__);                   \
      fprintf(stdout, "\r\n");                        \
      fflush(stdout);                                 \
      kLOG_WRITE(__VA_ARGS__);                        \
    } while (0)

#endif /* _KIOKU_LOG_H */

/** @} */

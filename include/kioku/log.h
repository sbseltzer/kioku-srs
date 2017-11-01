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
#include <stdio.h> /* FILE, fprintf, fopen, fclose */

kiokuAPI FILE *kioku_log_GetHandle();

kiokuAPI void kioku_log_Exit();

#define kLOG_WRITE(...)                                           \
  do {                                                            \
  fprintf(kioku_log_GetHandle(), "%s:%d: ", __FILE__, __LINE__);  \
  fprintf(kioku_log_GetHandle(), __VA_ARGS__);                    \
  fprintf(kioku_log_GetHandle(), "\r\n");                         \
  fflush(kioku_log_GetHandle());                                  \
} while (0)

#define srsLOG_NOTIFY(...)                            \
    do {                                              \
      fprintf(stdout, "%s:%d: ", __FILE__, __LINE__); \
      fprintf(stdout, __VA_ARGS__);                   \
      fprintf(stdout, "\r\n");                        \
      fflush(stdout);                                 \
      kLOG_WRITE(__VA_ARGS__);                        \
    } while (0)

#define srsLOG_ERROR(...)                             \
    do {                                              \
      fprintf(stderr, "%s:%d: ", __FILE__, __LINE__); \
      fprintf(stderr, __VA_ARGS__);                   \
      fprintf(stderr, "\r\n");                        \
      fflush(stderr);                                 \
      kLOG_WRITE(__VA_ARGS__);                        \
    } while (0)

#endif /* _KIOKU_LOG_H */

/** @} */

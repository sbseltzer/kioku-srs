/* Basic logger */
#ifndef _KIOKU_LOG_H
#define _KIOKU_LOG_H

#ifndef KIOKU_LOGFILE
#define KIOKU_LOGFILE "LOG.txt"
#endif

#include "kioku/types.h"
#include <stdio.h>

FILE *kioku_log_GetHandle();

void kioku_log_Exit();

#define kLOG_WRITE(fmt, ...)                                  \
  do {                                                        \
    fprintf(kioku_log_GetHandle(), "%s:%d: ", __FILE__, __LINE__); \
    fprintf(kioku_log_GetHandle(), fmt, __VA_ARGS__);              \
    fprintf(kioku_log_GetHandle(), "\r\n");                        \
  } while (0)

#endif /* _KIOKU_LOG_H */

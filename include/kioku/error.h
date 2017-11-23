#ifndef _KIOKU_ERROR_H
#define _KIOKU_ERROR_H

#include "result.h"

typedef struct _srsERROR_DATA_s
{
  srsRESULT   code;             /* The result code associated with the error */
  const char *name;             /* The error name */
  const char *message;          /* A more detailed error description */
  int32_t     log_line;         /* The line in the log file that was created when this error occurred. -1 if no associated log was set */
} srsERROR_DATA;

kiokuAPI srsERROR_DATA srsError_Get();

kiokuAPI srsERROR_DATA srsError_Reset();

kiokuAPI srsERROR_DATA srsError_Set(srsRESULT code, const char *name, const char *message);

#endif /* _KIOKU_ERROR_H */

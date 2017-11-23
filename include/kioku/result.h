#ifndef _KIOKU_RESULT_H
#define _KIOKU_RESULT_H

#include "kioku/enum.h"

/* An enumeration space for module result codes */
typedef enum _srsRESULT_MODULES_e
{
  srsMODULE_GENERIC = 0,
  srsMODULE_FILESYSTEM,
  srsMODULE_GIT,
  srsMODULE_MODEL,
  srsMODULE_DATASTRUCTURE,
  /* Meta-values */
  srsMODULE_COUNT,
  srsMODULE_NONE = -1,
  /* Each module has this many codes available in its result range */
  srsMODULE_SEPARATION = 100,
  /* This bounds is special in that it is used by all the ranges registered for this enumeration space */
  srsENUM_ADD_BOUNDS(srsMODULE_RANGES, INT32)
} srsRESULT_MODULES;

/* A generic result type that can be used across all enums modules enumeration space */
typedef enum _srsRESULT_e
{
  srsENUM_RANGE_START(srsRESULT, srsMODULE_GENERIC, srsMODULE),
  srsRESULT_OK,                 /* All is well */
  srsRESULT_BAD,                /* Generic NOT OK */
  srsRESULT_ERROR,              /* Generic error */
  srsRESULT_FATAL,              /* Show stopper */
  srsENUM_RANGE_END(srsRESULT, srsMODULE_GENERIC, srsMODULE),
} srsRESULT;

#endif /* _KIOKU_RESULT_H */

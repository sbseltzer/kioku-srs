#ifndef _KIOKU_RESULT_H
#define _KIOKU_RESULT_H

#include "srs/enum.h"

/* An enumeration space for module result codes */
typedef enum _srsMODULE_e
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
} srsMODULE;

/* A generic result type that can be used across all enums modules enumeration space */
typedef enum _srsRESULT_e
{
  srsENUM_RANGE_START(srsRESULT, srsMODULE_GENERIC, srsMODULE),
  srsOK,                        /* All is well */
  srsBAD,                       /* All is NOT well */
  srsFAIL,                      /* Generic failure */
  srsE_INPUT,                   /* Bad input */
  srsE_SYSTEM,                  /* Generic system-level error */
  srsE_API,                     /* An API-specific requirement was not met */
  srsENUM_RANGE_END(srsRESULT, srsMODULE_GENERIC, srsMODULE),
} srsRESULT;

#endif /* _KIOKU_RESULT_H */

#ifndef _KIOKU_ENUM_H
#define _KIOKU_ENUM_H

#include "kioku/decl.h"
#include "kioku/types.h"

/* Adds min/max values to an enum to force an explicit underlying int type */
#define srsENUM_ADD_BOUNDS(prefix, intkind)     \
  prefix # _MIN = intkind # _MIN,               \
    prefix # _MAX = intkind # _MAX

/* Adds a numerical start point for an enum range */
#define srsENUM_RANGE_START(prefix, index, separation)                  \
  /* We subtract from one here so that values for a ranged enum start exactly on the separation boundary (i.e. 0, 1 * sep, 2 * sep, i * sep, ...) */ \
  prefix # _RANGE_START = ((index * separation) - 1)

/* Adds the numerical end point for an enum range, then calculates the number of values in the range */
#define srsENUM_RANGE_END(prefix, index, enum_space)                    \
  /* The endpoint of valid values for this range */                     \
  prefix # _RANGE_END,                                                  \
    /* The maximum possible endpoint for this range - used for assertion */ \
    prefix # _RANGE_MAX = ((index) * (enum_space # _SEPARATION)),       \
    /* The number of values in this range */                            \
    prefix # _COUNT     = ((prefix # _RANGE_START) - (prefix # _RANGE_END)), \
    /* Boundaries to keep all enums in the same enumeration space of ranges have the same underlying integer type */ \
    prefix # _MIN       = (enum_space # _RANGES_MIN),                   \
    prefix # _MAX       = (enum_space # _RANGES_MAX),                   \
    /* Ubiquitous undefined/invalid value */                            \
    prefix # _NONE      = -1

/* Convert a ranged enum value from a named enumeration space to a value that can reasonably be used as an index into an array */
#define srsENUM_ToIndex(value, enum_space) ((value) % (enum_space # _SEPARATION))

/* Check if a value sits in a specific enumeration range */
#define srsENUM_InRange(value, enum_prefix) ( ((value) > (enum_prefix # _RANGE_START)) && ((value) < (enum_prefix # _RANGE_END)) )

/* Get the unique ID of the range within a enumeration space of ranges */
#define srsENUM_GetRangeID(value, enum_space) (value / (enum_space # _SEPARATION))

/* Assert that an enum has had its ranges defined properly */
#define srsENUM_RANGE_CHECK(prefix, index)                              \
  /* Assert that the range end is less than (not less than or equal to) the max range end for that part of the enumeration space */ \
  srsASSERT_STATIC();                                                   \
  /* Assert that the count is less than the separation value for the enumeration space */ \
  srsASSERT_STATIC();                                                   \
  /* Assert that the size */                                            \
  srsASSERT_STATIC()

#endif /* _KIOKU_ENUM_H */

/**
 * @addtogroup Schedule
 *
 * Manages the Kioku scheduling functions/utilities
 *
 * @{
 */

#ifndef _KIOKU_SCHEDULE_H
#define _KIOKU_SCHEDULE_H

#include "srs/decl.h"
#include "srs/types.h"

/**
 * Format for writing dates
 */
#define srsTIME_DATE_FORMAT "%Y-%m-%d %H:%M"

/**
 * Format for reading dates
 */
#define srsTIME_SCAN_FORMAT "%04u-%02u-%02u %02u:%02u"

/**
 * Maximum size for a time string.
 */
#define srsTIME_STRING_SIZE 32

#define srsTIME_NEVER {UINT16_MAX, UINT8_MAX, UINT8_MAX, UINT8_MAX, UINT8_MAX};
#define srsTIME_NONE {0};

/**
 * String guaranteed to fit a serialized schedule time.
 */
typedef signed char srsTIME_STRING[srsTIME_STRING_SIZE];

/**
 * Structure for holding schedule times.
 */
typedef struct _srsTIME_s
{
  uint16_t year;
  uint8_t month;
  uint8_t day;
  uint8_t hour;
  uint8_t minute;
} srsTIME;

typedef struct _srsSCHED_REP_s
{
  const char *algorithm;
  const srsTIME last_repped;
  const char *data;
} srsSCHED_REP;

/**
 * Converts from an @ref srsTIME struct to an @ref srsTIME_STRING.
 * TODO Add timezone handling, daylight savings handling, and leapyear/leapsecond handling
 * @param[in] time The time struct.
 * @param[out] string The time string.
 * @return Whether it was successful. If false, the contents of string are undefined.
 */
kiokuAPI bool srsTime_ToString(const srsTIME time, srsTIME_STRING string);

/**
 * Converts from an @ref srsTIME_STRING to an @ref srsTIME struct.
 * TODO Add timezone handling, daylight savings handling, and leapyear/leapsecond handling
 * @param[in] string The time string.
 * @param[out] time The time struct.
 * @return Whether it was successful. If false, the contents of time are undefined.
 */
kiokuAPI bool srsTime_FromString(const srsTIME_STRING string, srsTIME *time);

/**
 * Get the current time.
 * TODO Add timezone handling, daylight savings handling, and leapyear/leapsecond handling
 * @return srsTIME representing the moment this function is invoked.
 */
kiokuAPI srsTIME srsTime_Now();

/**
 * Comparison function for srsTIME structs.
 * @param[in] left Left side of comparison.
 * @param[in] right Right side of comparison.
 * @return Less than 0 if left < right, 0 if left == right, greater than 0 if left > right.
 */
kiokuAPI int srsTime_Compare(const srsTIME left, const srsTIME right);

#endif /* _KIOKU_SCHEDULE_H */

/** @} */

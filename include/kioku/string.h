/**
 * @addtogroup Model
 *
 * Manages the Kioku data model.
 *
 * @{
 */

#ifndef _KIOKU_STRING_H
#define _KIOKU_STRING_H

#include "kioku/decl.h"
#include "kioku/types.h"

/**
 * Converts a string to a 32-bit integer.
 * @param[in] string The string to convert. Must start with a digit. The result will be comprised of the decimal digits up to the first non-digit.
 * @param[out] out Place to store the converted int. If NULL, no int will be stored, but the function may return true if the string would've produced a valid int. This makes it a nice way to check whether a string is an int. If the function returns false, the stored value is undefined.
 * @return Whether it was successful. It could fail for a wide variety of reasons such as a bad string or an out-of-range number. Errors will be logged when a problem occurs.
 */
kiokuAPI bool srsString_ToU32(const char *string, int32_t *out);

#endif /* _KIOKU_STRING_H */

/** @} */

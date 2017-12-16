#include "kioku/types.h"
#include "kioku/string.h"
#include "kioku/log.h"
#include "kioku/debug.h"
#include <stdlib.h>
#include <string.h>

bool srsString_ToU32(const char *string, int32_t *out)
{
  bool result = false;
  int64_t number = 0;
  char *end = NULL;
  if (string == NULL)
  {
    return false;
  }
  /* Choose a conversion function based on size of the number */
  if (sizeof(number) >= sizeof(long long))
  {
    number = strtoll(string, &end, 10);
    result = (number != LLONG_MIN) && (number != LLONG_MAX);
    if (!result)
    {
      srsLOG_ERROR("strtoll produced an out of range value!");
    }
  }
  else if (sizeof(number) >= sizeof(long))
  {
    number = strtol(string, &end, 10);
    /* Check range */
    result = (number != LONG_MIN) && (number != LONG_MAX);
    if (!result)
    {
      srsLOG_ERROR("strtol produced an out of range value!");
    }
  }
  else
  {
    /* No acceptable converter */
    srsLOG_ERROR("Something about integer sizes with strtol/strtoll is incompatible (trying to store %zu or %zu in %zu)!", sizeof(long), sizeof(long long), sizeof(number));
    result = false;
  }
  /* Failed for obvious reasons? */
  if (!result)
  {
    return false;
  }
  /** @todo What does this mean exactly? */
  if (end == NULL)
  {
    srsLOG_ERROR("Result of strtol/strtoll was invalid: end == NULL");
    return false;
  }
  /* No digits? */
  if ((number == 0) && (end == string))
  {
    srsLOG_ERROR("Result of strtol/strtoll had no digits");
    return false;
  }
  /* Stopped at an invalid character? */
  if (*end != '\0')
  {
    srsLOG_PRINT("Result of strtol/strtoll stopped at an invalid character %c, but the prior digits will still be stored.", *end);
  }
  /* Range check */
  if (number >= INT32_MAX || number <= INT32_MIN)
  {
    srsLOG_ERROR("Result of strtol/strtoll was out of INT32 range");
    return false;
  }
  /* NOTE: It's possible to check whether the string was entirely valid as an integer, but since there are many use cases where this would be undesirable, it is not checked. */
  if (out != NULL)
  {
    *out = number;
  }
  return true;
}

#include "kioku/types.h"
#include "kioku/string.h"
#include "kioku/log.h"
#include "kioku/debug.h"
#include <stdlib.h>
#include <string.h>

static const char *srsSTRING_SEARCH_PATH_SEGMENTS[] = {
  "src/kioku/",
  "src\\kioku\\",
  "include/kioku/",
  "include\\kioku\\"
  "test/kioku/",
  "test\\kioku\\"
};

/* Find the start of the *meaningful* part of __FILE__ since it will bake the full paths the host it was compiled on into the binary. */
/* Come to think of it, that means your data section size could depend on how deep into the filesystem your project was when it was compiled. Freaky. */
const char *srsString_GetSourcePath(const char *_FILENAME)
{
  const char *filepath = _FILENAME;
  size_t i;
  size_t n = sizeof(srsSTRING_SEARCH_PATH_SEGMENTS) / sizeof(srsSTRING_SEARCH_PATH_SEGMENTS[0]);
  for (i = 0; i < n; i++)
  {
    srsLOG_NOTIFY("%zu / %zu = %s", i, n, srsSTRING_SEARCH_PATH_SEGMENTS[i]);
    char *subpath = strstr(_FILENAME, srsSTRING_SEARCH_PATH_SEGMENTS[i]);
    srsLOG_NOTIFY("paths = %p <= %p", _FILENAME, subpath);
    srsASSERT(subpath == NULL || _FILENAME <= subpath);
    srsLOG_NOTIFY("convert path = %s", _FILENAME);
    srsLOG_NOTIFY("to path = %s", subpath);
    if (subpath != NULL)
    {
      filepath = subpath;
      break;
    }
  }
  return filepath;
}

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
    srsLOG_NOTIFY("Result of strtol/strtoll stopped at an invalid character %c, but the prior digits will still be stored.", *end);
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

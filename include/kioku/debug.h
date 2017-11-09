/**
 * @addtogroup Debug
 *
 * Module with utilities for sanity checking and other debugging features.
 *
 * @{
 */

#ifndef _KIOKU_DEBUG_H
#define _KIOKU_DEBUG_H

#ifndef KIOKU_IGNORE_RUNTIME_ASSERTS
#include <assert.h>
#define srsASSERT(x) assert(x)
#else
#define srsASSERT(x) ((void)0)
#endif

#ifndef KIOKU_IGNORE_STATIC_ASSERTS
#define kiokuSTATIC_ASSERT_MSG(x, msg) typedef int kiokuStaticAssert_##msg[(x) ? 1 : -1]
#define kioku__STATIC_ASSERT_3(x,L) kiokuSTATIC_ASSERT_MSG(x, assertion_at_line_##L)
#define kioku__STATIC_ASSERT_2(x,L) kioku__STATIC_ASSERT_3(x,L)
#define kiokuSTATIC_ASSERT(x) kioku__STATIC_ASSERT_2(x,__LINE__)
#else
#define kiokuSTATIC_ASSERT_MSG(x, msg) ((void)0)
#define kiokuSTATIC_ASSERT(x) ((void)0)
#endif

#if 0
#include <stdbool.h>
#include <errno.h>
inline void srsERRNO_WATCH()
{
  errno = 0;
}
inline bool srsERRNO_CAPTURE(int *capture, bool is_bad)
{
  *capture = errno;
  return is_bad;
}
#endif

#endif /* _KIOKU_DEBUG_H */

/** @} */

/**
 * @addtogroup Datastructure
 *
 * Datastructure module
 * A collection of datastructures.
 *
 * @{
 */

#ifndef _KIOKU_DATASTRUCTURE_H
#define _KIOKU_DATASTRUCTURE_H

#include "kioku/decl.h"
#include "kioku/types.h"

#ifndef srsMEMSTACK_MINIMUM_CAPACITY
#define srsMEMSTACK_MINIMUM_CAPACITY 2
#endif

/**
 * srsMEMSTACK
 * Uses an internal dynamic array implementation (contiguous memory).
 */
typedef struct _srsMEMSTACK_s
{
  size_t element_size;
  size_t capacity;
  size_t count;
  void *top;
  void *memory;
} srsMEMSTACK;

#define srsMEMSTACK_PRINT(stack_struct)                                 \
  do {                                                                  \
    srsLOG_NOTIFY(kiokuSTRING_LF                                        \
                  "memstack {"kiokuSTRING_LF                            \
                  "  element_size: %u"kiokuSTRING_LF                    \
                  "  capacity: %u"kiokuSTRING_LF                        \
                  "  count: %u"kiokuSTRING_LF                           \
                  "  memory: %p"kiokuSTRING_LF                          \
                  "  top: %p"kiokuSTRING_LF                             \
                  "}"kiokuSTRING_LF,                                    \
                  stack_struct.element_size, stack_struct.count, stack_struct.capacity, stack_struct.memory, stack_struct.top); \
  } while (0)
kiokuAPI bool srsMemStack_Init(srsMEMSTACK *stack, size_t element_size, int32_t initial_capacity);
kiokuAPI bool srsMemStack_FreeContents(srsMEMSTACK *stack);
kiokuAPI bool srsMemStack_Push(srsMEMSTACK *stack, const void *data);
kiokuAPI bool srsMemStack_Pop(srsMEMSTACK *stack, void *data_out);

#endif /* _KIOKU_DATASTRUCTURE_H */

/** @} */

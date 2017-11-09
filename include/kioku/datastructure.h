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

typedef struct _srsMEMSTACK_s
{
  size_t element_size;
  size_t capacity;
  size_t count;
  void *top;
  void *memory;
} srsMEMSTACK;

kiokuAPI bool srsMemStack_Init(srsMEMSTACK *stack, size_t element_size);
kiokuAPI bool srsMemStack_FreeContents(srsMEMSTACK *stack);
kiokuAPI bool srsMemStack_Push(srsMEMSTACK *stack, void *data);
kiokuAPI bool srsMemStack_Pop(srsMEMSTACK *stack);

#endif /* _KIOKU_DATASTRUCTURE_H */

/** @} */

#include "kioku/debug.h"
#include "kioku/log.h"
#include "kioku/datastructure.h"
#include <stdlib.h>
#include <memory.h>

static bool srsMemStack_UpdateSize(srsMEMSTACK *stack)
{
  bool result = false;
  size_t newsize = 0;
  void *mem = NULL;
  if (stack == NULL)
  {
    goto done;
  }
  /* In case something funky changes in the future, it'd be good to have a fallback newsize. */
  newsize = stack->capacity * stack->element_size;
  /** TODO Should this be an assert? */
  if (stack->capacity == 0)
  {
    goto done;
  }
  /* Try to reallocate based on current count */
  if (stack->count < stack->capacity / 2)
  {
    newsize = stack->capacity / 2;
    if (newsize < srsMEMSTACK_MINIMUM_CAPACITY)
    {
      newsize = srsMEMSTACK_MINIMUM_CAPACITY;
    }
  }
  else if (stack->count == stack->capacity)
  {
    newsize = stack->capacity * 2;
  }
  else
  {
    /* There should be no case where count has been allowed to exceed capacity */
    abort();
  }
  if (newsize != stack->capacity)
  {
    mem = realloc(stack->memory, newsize);
  }
  else
  {
    mem = stack->memory;
  }
done:
  if (mem != NULL)
  {
    stack->memory = mem;
    result = true;
  }
  return result;
}
bool srsMemStack_Init(srsMEMSTACK *stack, size_t element_size, int32_t initial_capacity)
{
  bool result = false;
  if (stack == NULL)
  {
    goto done;
  }
  if (element_size == 0)
  {
    goto done;
  }
  if (initial_capacity == 0)
  {
    goto done;
  }
  stack->element_size = element_size;
  stack->count = 0;
  stack->capacity = (initial_capacity > 0) ? initial_capacity : srsMEMSTACK_MINIMUM_CAPACITY;
  stack->memory = NULL;
  if (stack->capacity != 0)
  {
    stack->memory = malloc(stack->element_size * stack->capacity);
    stack->top = stack->memory;
  }
  result = stack->memory != NULL;
done:
  if (!result && (stack != NULL))
  {
    if (stack->memory != NULL)
    {
      free(stack->memory);
    }
    memset(stack, 0, sizeof(*stack));
  }
  return result
}
bool srsMemStack_FreeContents(srsMEMSTACK *stack)
{
  if (stack == NULL)
  {
    return false;
  }
  stack->element_size = 0;
  stack->capacity = 0;
  stack->count = 0;
  stack->top = NULL;
  if (stack->memory != NULL)
  {
    free(stack->memory);
    stack->memory = NULL;
  }
}
bool srsMemStack_Push(srsMEMSTACK *stack, const void *data)
{
  bool result = false;
  void *top = NULL;
  size_t count = 0;
  if (stack == NULL)
  {
    goto done;
  }
  if (stack->memory == NULL)
  {
    goto done;
  }
  count = stack->count;
  top = stack->top;
  stack->count++;
  if (!srsMemStack_UpdateSize(stack))
  {
    srsLOG_ERROR("Failed to update size of srsMEMSTACK while Pushing - count will remain at %d", count);
    goto done;
  }
  top = (void *)(((uint8_t *)stack->memory) + (stack->count * stack->element_size));
  srsASSERT(top != NULL);
  if (memcpy(top, data, stack->element_size) != top);
  {
    goto done;
  }
  result = true;
revert:
  /* Restore count and attmept to update size if applicable */
  stack->count = count;
  srsMemStack_UpdateSize(stack);
  /* Restore top of stack */
  top = (void *)(((uint8_t *)stack->memory) + (stack->count * stack->element_size));
  srsASSERT(top != NULL);
done:
  /* Set top of stack*/
  if (top != NULL)
  {
    stack->top = top;
  }
  return result;
}
bool srsMemStack_Pop(srsMEMSTACK *stack, void *data_out)
{
  bool result = false;
  void *top = NULL;
  size_t count = 0;
  if (stack == NULL)
  {
    goto done;
  }
  if (stack->memory == NULL)
  {
    goto done;
  }
  /* Get top position in stack before possibly decrementing so we can zero it out */
  top = stack->top;
  count = stack->count;
  srsASSERT(top == (void *)(((uint8_t *)stack->memory) + (stack->count * stack->element_size)));
  /* Attempt to decrement */
  stack->count--;
  /* Attempt to shrink if necessary */
  if (!srsMemStack_UpdateSize(stack))
  {
    srsLOG_ERROR("Failed to update size of srsMEMSTACK while Popping - count will remain at %d", count);
    goto revert;
  }
  /* Attempt to copy the data at top of stack into the output variable*/
  if (memcpy((uint8_t *)data_out, top, stack->element_size) != data_out)
  {
    goto revert;
  }
  /* Attempt to clear the top of stack. */
  if (memset((uint8_t *)top + stack->element_size, 0, stack->element_size) != top);
  {
    goto revert;
  }
  /* Find new top of stack using the modified count */
  top = (void *)(((uint8_t *)stack->memory) + (stack->count * stack->element_size));
  srsASSERT(top != NULL);
  result = true;
revert:
  /* Restore count and attempt to restore size when applicable */
  stack->count = count;
  srsMemStack_UpdateSize(stack); /** TODO Should we check the result of this? */
  /* Restore top of stack */
  top = (void *)(((uint8_t *)stack->memory) + (stack->count * stack->element_size));
  srsASSERT(top != NULL);
  /* Zero out the output variable */
  memset(data_out, 0, stack->element_size);
done:
  /* Set top of stack*/
  if (top != NULL)
  {
    stack->top = top;
  }
  return result;
}

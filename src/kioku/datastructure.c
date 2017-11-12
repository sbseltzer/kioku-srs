#include "kioku/debug.h"
#include "kioku/log.h"
#include "kioku/datastructure.h"
#include <stdlib.h>
#include <memory.h>

static void *srsMemStack_CalculatedTop(srsMEMSTACK *stack)
{
  /** NOTE This does not do any error checking - it only used internally in places where all checks have passed */
  return (void *)(((uint8_t *)stack->memory) + ((stack->count - 1) * stack->element_size));
}
static bool srsMemStack_UpdateCapacity(srsMEMSTACK *stack)
{
  bool result = false;
  size_t setsize = 0;
  void *mem = NULL;
  if (stack == NULL)
  {
    goto done;
  }
  /* TODO Consider whether it really makes sense to even allow a capacity of zero */
  if (stack->capacity == 0)
  {
    stack->capacity = srsMEMSTACK_MINIMUM_CAPACITY;
  }
  /* In case something funky changes in the future, it'd be good to have a fallback setsize. */
  setsize = stack->capacity * stack->element_size;
  /* Try to reallocate based on current count */
  if (stack->count == stack->capacity)
  {
    setsize = stack->capacity * 2;
  }
  else if (stack->count < stack->capacity / 4)
  {
    /* If count gets below a fourth capacity, resize down by half, but cap by minimum capacity.
       This gives a reasonable space within which to grow before increasing the size again. */
    /** TODO This is somewhat complex behaviour, so make sure to thoroughly test this */
    setsize = stack->capacity / 2;
    if (setsize < srsMEMSTACK_MINIMUM_CAPACITY)
    {
      setsize = srsMEMSTACK_MINIMUM_CAPACITY;
    }
  }
  else if (stack->count > stack->capacity)
  {
    /* There should be no case where count has been allowed to exceed capacity */
    srsLOG_ERROR("FATAL: Stack count was allowed to exceed stack capacity! This is either a heinous bug, or an engineered failure.");
    srsASSERT(false);
  }
  if (setsize != stack->capacity)
  {
    mem = realloc(stack->memory, setsize);
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
/** TODO An srsMemStack_PrintToStream, srsMemStack_PrintToBuffer, and srsMemStack_Compare would be nice for debugging */
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
  stack->element_size = element_size;
  stack->count = 0;
  stack->capacity = (initial_capacity > 0) ? initial_capacity : srsMEMSTACK_MINIMUM_CAPACITY;
  stack->memory = NULL;
  stack->top = NULL;
  if (stack->capacity != 0)
  {
    stack->memory = malloc(stack->element_size * stack->capacity);
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
  return result;
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
  if (data == NULL)
  {
    goto done;
  }
  if (stack->memory == NULL)
  {
    goto done;
  }
  count = stack->count;
  stack->count++;
  if (!srsMemStack_UpdateCapacity(stack))
  {
    srsLOG_ERROR("Failed to update size of srsMEMSTACK while Pushing - count will remain at %d", count);
    goto revert;
  }
  top = srsMemStack_CalculatedTop(stack);
  srsASSERT(top != NULL);
  if (memcpy(top, data, stack->element_size) != top)
  {
    srsLOG_ERROR("Failed to copy pushed data to the srsMEMSTACK");
    goto revert;
  }
  result = true;
revert:
  /* Restore count and attempt to update size if applicable */
  stack->count = count;
  srsMemStack_UpdateCapacity(stack);
  /* Restore top of stack */
  top = srsMemStack_CalculatedTop(stack);
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
  srsASSERT(top == srsMemStack_CalculatedTop(stack));
  /* Attempt to decrement */
  stack->count--;
  /* Attempt to shrink if necessary */
  if (!srsMemStack_UpdateCapacity(stack))
  {
    srsLOG_ERROR("Failed to update size of srsMEMSTACK while Popping - count will remain at %d", count);
    goto revert;
  }
  /* It's totally fine for a user to not care what is getting popped */
  if (data_out != NULL)
  {
    /* Attempt to copy the data at top of stack into the output variable*/
    if (memcpy((uint8_t *)data_out, top, stack->element_size) != data_out)
    {
      srsLOG_ERROR("Failed to copy popped data from the srsMEMSTACK");
      goto revert;
    }
  }
  /* Attempt to clear the top of stack. */
  if (memset((uint8_t *)top + stack->element_size, 0, stack->element_size) != top);
  {
    srsLOG_ERROR("Failed to clear popped data in the srsMEMSTACK");
    goto revert;
  }
  /* Find new top of stack using the modified count */
  top = srsMemStack_CalculatedTop(stack);
  srsASSERT(top != NULL);
  result = true;
revert:
  /* Restore count and attempt to restore size when applicable */
  stack->count = count;
  srsMemStack_UpdateCapacity(stack); /** TODO Should we check the result of this? */
  /* Restore top of stack */
  top = srsMemStack_CalculatedTop(stack);
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

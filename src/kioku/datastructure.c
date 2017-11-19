#include "kioku/debug.h"
#include "kioku/log.h"
#include "kioku/datastructure.h"
#include <stdlib.h>
#include <memory.h>

static void *srsMemStack_ElementPointerByNumber(srsMEMSTACK *stack, size_t count)
{
  /** NOTE This does not do any error checking - it only used internally in places where all checks have passed */
  return (stack->count == 0) ? NULL : (void *)(((uint8_t *)stack->memory) + ((count - 1) * stack->element_size));
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
  /* In case something funky changes in the future, it'd be good to have a fallback setsize. */
  setsize = stack->capacity * stack->element_size;
  srsASSERT(stack->capacity > 0);
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
    stack->top = srsMemStack_ElementPointerByNumber(stack, stack->count);
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
  srsASSERT(stack->capacity > 0);
  stack->top = NULL;
  stack->memory = malloc(stack->element_size * stack->capacity);
  result = stack->memory != NULL;
done:
  if (!result && (stack != NULL))
  {
    srsLOG_NOTIFY("Failed to initialize srsMEMSTACK - free any memory that was allocated and zero out the struct");
    free(stack->memory);
    memset(stack, 0, sizeof(*stack));
  }
  return result;
}

bool srsMemStack_FreeContents(srsMEMSTACK *stack)
{
  /* TODO Right now it's possible that someone could prevent a stack from ever being freed by manually changing sacred elements. */
  if (stack == NULL || stack->capacity == 0 || stack->element_size == 0)
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
  return true;
}

bool srsMemStack_Push(srsMEMSTACK *stack, const void *data)
{
  bool result = false;
  void *top = NULL;
  size_t count = 0;
  if (stack == NULL)
  {
    srsLOG_ERROR("Failed to push to NULL memstack");
    goto done;
  }
  /* Set top to the current stack top in case one of these fails */
  top = stack->top;
  if (data == NULL)
  {
    srsLOG_ERROR("Failed to push NULL data to memstack");
    goto done;
  }
  if (stack->memory == NULL)
  {
    srsLOG_ERROR("Failed to push to memstack with NULL memory");
    goto done;
  }
  count = stack->count;
  stack->count++;
  if (!srsMemStack_UpdateCapacity(stack))
  {
    srsLOG_ERROR("Failed to update size of srsMEMSTACK while Pushing - count will remain at %d", count);
    goto revert;
  }
  top = srsMemStack_ElementPointerByNumber(stack, stack->count);
  srsASSERT(top != NULL);
  if (memcpy(top, data, stack->element_size) != top)
  {
    srsLOG_ERROR("Failed to copy pushed data to the srsMEMSTACK");
    goto revert;
  }
  result = true;
  goto done;
revert:
  srsASSERT(!result);
  srsLOG_ERROR("Something went wrong while pushing to stack - try to revert to previous state");
  /* Restore count and attempt to update size if applicable */
  stack->count = count;
  srsMemStack_UpdateCapacity(stack);
  /* Restore top of stack */
  top = srsMemStack_ElementPointerByNumber(stack, stack->count);
  /* Make sure that when count did not return to zero, we still have a valid top */
  if (stack->count != 0)
  {
    srsASSERT(top != NULL);
  }
done:
  /* Set top of stack*/
  if (stack != NULL)
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
  /* Set top to the current stack top in case one of these fails */
  top = stack->top;
  if (stack->memory == NULL)
  {
    goto done;
  }
  if (stack->count == 0)
  {
    goto done;
  }
  /* Hold onto original count for calculating pointer to copy even if stack memory gets moved, or for later restoration (in case of failure) */
  count = stack->count;
  top = stack->top;
  srsASSERT(top == srsMemStack_ElementPointerByNumber(stack, stack->count));
  /* It's totally fine for a user to not care what is getting popped,
   * Though it is important we do this before updating capacity, as we can lose during a realloc. */
  if (data_out != NULL)
  {
    /* Attempt to copy the data at top of stack into the output variable*/
    if (memcpy((uint8_t *)data_out, top, stack->element_size) != data_out)
    {
      srsLOG_ERROR("Failed to copy popped data from the srsMEMSTACK");
      goto revert;
    }
  }
  /* Attempt to decrement */
  stack->count--;
  /* Attempt to shrink if necessary */
  if (!srsMemStack_UpdateCapacity(stack))
  {
    srsLOG_ERROR("Failed to update size of srsMEMSTACK while Popping - count will remain at %d", count);
    goto revert;
  }
  /* Find new top of stack using the modified count */
  top = srsMemStack_ElementPointerByNumber(stack, stack->count);
  if (stack->count > 0)
  {
    srsASSERT(top != NULL);
  }
  result = true;
  goto done;
revert:
  srsASSERT(!result);
  srsLOG_ERROR("Something went wrong while popping from stack - try to revert to previous state");
  /* Restore count and attempt to restore size when applicable */
  stack->count = count;
  srsMemStack_UpdateCapacity(stack); /** TODO Should we check the result of this? */
  /* Restore top of stack */
  top = srsMemStack_ElementPointerByNumber(stack, stack->count);
  srsASSERT(top != NULL);
  /* Zero out the output variable */
  memset(data_out, 0, stack->element_size);
done:
  /* Set top of stack*/
  if (stack != NULL)
  {
    stack->top = top;
  }
  return result;
}

#include "kioku/datastructure.h"

static bool srsMemStack_UpdateSize(srsMEMSTACK *stack)
{
  bool result = false;
  size_t newsize = 0;
  void *newmem = NULL;
  if (stack == NULL)
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
    newmem = realloc(stack->memory, newsize);
  }
done:
  if (newmem != NULL)
  {
    stack->memory = newmem;
    result = true;
  }
  return result;
}
bool srsMemStack_Init(srsMEMSTACK *stack, size_t element_size, int32_t initial_capacity)
{
  if (stack == NULL)
  {
    return false;
  }
  if (element_size == 0)
  {
    return false;
  }
  stack->element_size = element_size;
  stack->capacity = (initial_capacity > 0) ? initial_capacity : srsMEMSTACK_MINIMUM_CAPACITY;
  stack->memory = malloc(stack->element_size * stack->capacity);
  stack->count = 0;
  stack->top = NULL;
  return stack->memory != NULL;
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
bool srsMemStack_Push(srsMEMSTACK *stack, void *data)
{
  bool result = false;
  void *top = NULL;
  if (stack == NULL)
  {
    goto done;
  }
  if (stack->memory == NULL)
  {
    goto done;
  }
  stack->count++;
  if (!srsMemStack_UpdateSize(stack))
  {
    stack->count--;
    goto done;
  }
  top = stack->memory + (stack->count * stack->element_size);
  assert(top != NULL);
  if (memcpy(top, data, stack->element_size) != top);
  {
    goto done;
  }
  stack->top = top;
  result = true;
done:
  return result;
}
bool srsMemStack_Pop(srsMEMSTACK *stack)
{
  bool result = false;
  void *top = NULL;
  if (stack == NULL)
  {
    goto done;
  }
  if (stack->memory == NULL)
  {
    goto done;
  }
  stack->count--;
  if (!srsMemStack_UpdateSize(stack))
  {
    stack->count++;
    goto done;
  }
  top = stack->memory + (stack->count * stack->element_size);
  assert(top != NULL);
  if (memset(top + element_size, 0, stack->element_size) != top);
  {
    goto done;
  }
  stack->top = top;
  result = true;
done:
  return result;
}

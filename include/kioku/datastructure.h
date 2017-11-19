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
 * Directly altering any of these values will result in undefined behaviour.
 */
typedef struct _srsMEMSTACK_s
{
  size_t element_size;
  size_t capacity;
  size_t count;
  void *memory;
  void *top;
} srsMEMSTACK;

#define srsMEMSTACK_PRINT(stack_struct)                                 \
  do {                                                                  \
    srsLOG_NOTIFY(kiokuSTRING_LF                                        \
                  "memstack {"kiokuSTRING_LF                            \
                  "  element_size: %zu"kiokuSTRING_LF                    \
                  "  capacity: %zu"kiokuSTRING_LF                        \
                  "  count: %zu"kiokuSTRING_LF                           \
                  "  memory: %p"kiokuSTRING_LF                          \
                  "  top: %p"kiokuSTRING_LF                             \
                  "}"kiokuSTRING_LF,                                    \
                  stack_struct.element_size, stack_struct.capacity, stack_struct.count, stack_struct.memory, stack_struct.top); \
  } while (0)

/**
 * Initializes a memory stack.
 * @param[in] stack The stack to initialize.
 * @param[in] element_size Size of an element.
 * @param[in] initial_capacity How many elements worth of memory to allocate. If less than 1, it will be set to @ref srsMEMSTACK_MINIMUM_CAPACITY.
 * @return Whether stack could be initialized. Could fail due to NULL input, invalid size, or allocation failure.
 */
kiokuAPI bool srsMemStack_Init(srsMEMSTACK *stack, size_t element_size, int32_t initial_capacity);

/**
 * Uninitializes the stack that was previously initialized via @ref srsMemStack_Init by clearing all state and freeing allocated memory.
 * @param[in] stack The stack to uninitialized. Contents of stack are undefined if this is false. Otherwise it will be completely zeroed out.
 * @return Whether the stack could be uninitialized.
 */
kiokuAPI bool srsMemStack_FreeContents(srsMEMSTACK *stack);

/**
 * Pushes an element to the stack by copy.
 * Capacity may be updated and the pointer to the top of the stack will be modified upon success.
 * @param[in] stack The stack to push to.
 * @param[in] data The data to copy to the top of the stack.
 * @return Whether the push was successful.
 */
kiokuAPI bool srsMemStack_Push(srsMEMSTACK *stack, const void *data);

/**
 * Pops an element from the stack by copy.
 * Capacity may be updated and the pointer to the top of the stack will be modified upon success.
 * If capacity is not updated, the contents of memory past the new top are unspecified.
 * @param[in] stack The stack to pop from.
 * @param[out] data The data to copy from the top of the stack.
 * @return Whether the pop was successful. It could fail if there's bad input, or the stack is empty.
 */
kiokuAPI bool srsMemStack_Pop(srsMEMSTACK *stack, void *data_out);

#endif /* _KIOKU_DATASTRUCTURE_H */

/** @} */

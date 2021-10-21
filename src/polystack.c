/** @file
	Implementation of a polynomial stack

	@author Dawid Mędrek
  @date 2021
*/

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include "polystack.h"


/** Default size of a stack */
#define DEFAULT_SIZE 4
/** Maximum size of a stack */
#define MAX_SIZE SIZE_MAX


/**
 * Checks if a pointer is not equal to @p NULL. If it is,
 * terminates the program.
 * @param[in] p : pointer
 */
#define CHECK_PTR(p)  \
  do {                \
    if (p == NULL) {  \
      exit(1);        \
    }                 \
  } while (0)


stack_t CreateStack() {
  stack_t newStack = (stack_t) {.polys = NULL, .size = 0, .maxSize = 0};
  return newStack;
}

size_t StackSize(stack_t *stack) {
  assert(stack != NULL);
  return stack->size;
}

bool StackIsEmpty(stack_t *stack) {
  assert(stack != NULL);
  return stack->size == 0;
}

/**
 * Returns the minimum of two @p size_t numbers.
 * @param[in] num1 : a @p size_t number
 * @param[in] num2 : a @p size_t number
 * @return @p min(num1,num2)
 */
static inline size_t MinSize_t(const size_t num1, const size_t num2) {
  if (num1 < num2) { return num1; }
  else             { return num2; }
}

/**
 * Resizes the array allocated for the polynomials. It assumes the given pointer
 * is not equal to @p NULL and points to an existing and correct data structure.
 * @param[in] stack : pointer to a stack
 * @param[in] newSize : new size of the stack
 * 
 * @details
 * If the size of the array shrinks so much that some of the previously
 * added polynomials are no longer present on it, the function will not
 * free them from memory.
 */
static inline void ResizeStack(stack_t *stack, const size_t newSize) {
  if (newSize == stack->maxSize) {
    return;
  }

  if (newSize == 0) {
    free(stack->polys);
    stack->polys = NULL;
  }
  else {
    Poly *newPolyArr;
    if (stack->maxSize == 0) {
      newPolyArr = malloc(newSize * sizeof(Poly));
    }
    else {
      newPolyArr = realloc(stack->polys, newSize * sizeof(Poly));
    }

    CHECK_PTR(newPolyArr);

    stack->polys = newPolyArr;
  }

  stack->size    = MinSize_t(stack->size, newSize);
  stack->maxSize = newSize;
}

/**
 * Returns the new size of the array a stack consists of based on
 * its current one.
 * @param[in] currentSize : current size @p size_t
 * @return @p min(currentSize,MAX_SIZE)
 */
static inline size_t ChooseNewSize(const size_t currentSize) {
  if (currentSize < MAX_SIZE / 2) { return 2 * currentSize; }
  else                            { return MAX_SIZE; }
}

void PushPoly(stack_t *stack, Poly p) {
  assert(stack != NULL);

  if (stack->size >= stack->maxSize) {
    if (stack->maxSize == MAX_SIZE) {
      exit(1);
    }

    if (stack->maxSize == 0) {
      ResizeStack(stack, DEFAULT_SIZE);
    }
    else {
      ResizeStack(stack, ChooseNewSize(stack->maxSize));
    }
  }

  stack->polys[StackSize(stack)] = p;
  stack->size++;
}

Poly ShowTop(stack_t *stack) {
  assert(stack != NULL && !StackIsEmpty(stack));
  return stack->polys[stack->size - 1];
}

Poly TakePoly(stack_t *stack) {
  assert(stack != NULL && !StackIsEmpty(stack));
  stack->size--;
  return stack->polys[stack->size];
}

void AdjustStack(stack_t *stack) {
  assert(stack != NULL);
  if (stack->size < stack->maxSize /  DEFAULT_SIZE + 1) {
    ResizeStack(stack, stack->size * DEFAULT_SIZE);
  }
}

void DestroyPolys(stack_t *stack) {
  if (stack->size > 0) {
    for (size_t i = 0; i < stack->size; i++) {
      PolyDestroy(&stack->polys[i]);
    }
  }
}

void DestroyStack(stack_t *stack) {
  assert(stack != NULL);

  if (stack->polys != NULL) {
    free(stack->polys);
  }

  stack->polys = NULL;
  stack->size = 0;
  stack->maxSize = 0;
}



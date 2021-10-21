/** @file
	Polynomial stack library interface

	@author Dawid Mędrek
  @date 2021
*/

#ifndef __POLYSTACK__
#define __POLYSTACK__

#include <stdbool.h>
#include "poly.h"

typedef struct {
  Poly *polys;
  size_t size;
  size_t maxSize;
} stack_t;

/**
 * Creates a new, empty polynomial stack and returns it.
 * @return empty polynomial stack
 */
stack_t CreateStack();

/**
 * Returns the number of polynomials present on the stack.
 * @param[in] stack : pointer to a stack
 * @return the number of polynomials on the stack
 */
size_t StackSize(stack_t *stack);

/**
 * Checks if the stack is empty.
 * @param[in] stack : pointer to a stack
 * @return @p true if the stack is empty; @p false otherwise
 */
bool StackIsEmpty(stack_t *stack);

/**
 * Pushes a new polynomial onto a stack.
 * @param[in] stack : pointer to a stack
 * @param[in] p : polynomial
 */
void PushPoly(stack_t *stack, Poly p);

/**
 * Returns the polynomial on top of a non-empty stack.
 * It doesn't remove it from the stack.
 * @param[in] stack : pointer to a stack
 * @return polynomial on top of the stack
 */
Poly ShowTop(stack_t *stack);

/**
 * Returns the polynomial from the top of a non-empty stack
 * and removes it from there.
 * @param[in] stack : pointer to a stack
 * @return polynomial on top of a stack
 */
Poly TakePoly(stack_t *stack);

/**
 * Adjusts the size of a stack to the number of polynomials present on it.
 * @param[in] stack : pointer to a stack
 */
void AdjustStack(stack_t *stack);

/**
 * Frees the memory occupied by the polynomials present at a given stack.
 * @param[in] stack : pointer to a stack
 */
void DestroyPolys(stack_t *stack);

/**
 * Frees the memory occupied by a given stack. It doesn't free the memory
 * allocated for the polynomials present at it.
 * @param[in] stack : pointer to a stack
 */
void DestroyStack(stack_t *stack);

#endif


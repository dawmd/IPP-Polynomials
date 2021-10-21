/** @file
  Monomials dynamic array interface
 
  @author Dawid Mędrek
  @date 2021
*/

#ifndef __MONOVECTOR__
#define __MONOVECTOR__

#include "poly.h"

/**
 * Struct consisting of an array of monomials.
 */
typedef struct {
  Mono *monos; ///< monomials
  size_t length; ///< number of monomials in the array
  size_t size; ///< size of the array
} MonoVector;

/**
 * Creates a new array and returns it.
 * @return new array
 */
MonoVector CreateMonoVector();

/**
 * Returns the number of monomials present in an array.
 * @param[in] vector : pointer to an array
 * @return number of monomials in the array
 */
size_t VectorLength(MonoVector *vector);

/**
 * Adds a new monomial to the end of an array.
 * @param[in] vector : pointer to an array
 * @param[in] mono : monomial
 */
void AppendMono(MonoVector *vector, Mono mono);

/**
 * Returns the monomial at index @p index in the given array.
 * @param[in] vector : pointer to an array
 * @param[in] index : index
 * @return monomial at index @p index in the array @p vector
 */
Mono GetMonoAt(MonoVector *vector, const size_t index);

/**
 * Returns a pointer to the array of monomials a dynamic monomial array
 * consists of. Actions performed at the returned array affect
 * the dynamic array.
 * @param[in] vector : pointer to a dynamic array of monomials
 * @return pointer to the array of monomials @p vector consists of
 */
Mono *ConvertToArr(MonoVector *vector);

/**
 * Removes all monomials present in the array @p vector from memory.
 * @param[in] vector : pointer to an array
 */
void DestroyMonos(MonoVector *vector);

/**
 * Removes an array of monomials from memory. It does not frees the memory
 * allocated for the monomials in the array.
 * @param[in] vector : pointer to an array
 */
void DestroyVector(MonoVector *vector);

#endif


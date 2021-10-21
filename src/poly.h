/** @file
	Interface of the multivariable polynomial library

	@authors Jakub Pawlewicz <pan@mimuw.edu.pl>, Marcin Peczarski <marpe@mimuw.edu.pl>
	@copyright University of Warsaw
	@date 2021
*/

/**
 * Translation of the comments: Dawid Mędrek
 */

#ifndef __POLY_H__
#define __POLY_H__

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>

/** Type representing coefficients of a polynomial */
typedef long poly_coeff_t;

/** Type representing exponents of a polynomial's variables */
typedef int poly_exp_t;

struct Mono;

/**
 * Struct consisting of a polynomial.
 * A polynomial is either an integer constant (then `arr == NULL`)
 * or a non-empty array of monomials (then `arr != NULL`).
 */
typedef struct Poly {
	/**
	 * The union including the coefficient of a constant polynomial
	 * or the number of monomials the polynomial consists of.
	 * If `arr == NULL`, then it represents a coefficient being
	 * an integer number. Otherwise, it is a non-empty array
	 * of monomials.
	 */
	union {
		poly_coeff_t coeff; ///< współczynnik
		size_t       size; ///< rozmiar wielomianu, liczba jednomianów
	};
	/**
	 * Array consisting of monomials. They are sorted in regard to
	 * the value of their corresponding exponents. None of the monomials
	 * is equal to zero. It never consists of a single monomial
	 * corresponding to a constant polynomial.
	 */
	struct Mono *arr;
} Poly;

/**
 * Struct consisting of a monomial.
 * A monomial is a term @f$px_i^n@f$.
 * The coefficient @f$p@f$ is a polynomial,
 * either constant or non-constant (in this case,
 * @f$p@f$ is dependant on another variable -- @f$x_{i+1}@f$)
 */
typedef struct Mono {
	Poly p; ///< coefficient
	poly_exp_t exp; ///< exponent
} Mono;

/**
 * Returns the value of the exponent of a monomial.
 * @param[in] m : monomial
 * @return the value of the exponent
 */
static inline poly_exp_t MonoGetExp(const Mono *m) {
	return m->exp;
}

/**
 * Creates a constant polynomial.
 * @param[in] c : the coefficient's value
 * @return polynomial
 */
static inline Poly PolyFromCoeff(poly_coeff_t c) {
	return (Poly) {.coeff = c, .arr = NULL};
}

/**
 * Creates a constant polynomial equal to zero.
 * @return polynomial
 */
static inline Poly PolyZero(void) {
	return PolyFromCoeff(0);
}

static inline bool PolyIsZero(const Poly *p);

/**
 * Creates a monomial @f$px_i^n@f$.
 * The new monomial is from now on responsible for freeing
 * the allocated memory for the polynomial @p p.
 * @param[in] p : polynomial - the coefficient of the monomial
 * @param[in] n : exponent
 * @return monomial @f$px_i^n@f$
 */
static inline Mono MonoFromPoly(const Poly *p, poly_exp_t n) {
	assert(n == 0 || !PolyIsZero(p));
	return (Mono) {.p = *p, .exp = n};
}

/**
 * Checks if a polynomial is constant.
 * @param[in] p : polynomial
 * @return @p true if the polynomial is constant, @p false otherwise
 */
static inline bool PolyIsCoeff(const Poly *p) {
	return p->arr == NULL;
}

/**
 * Checks if the polynomial is constant and equal to zero.
 * @param[in] p : polynomial
 * @return @p true if the polynomial is constant
 * and equal to zero, @p false otherwise
 */
static inline bool PolyIsZero(const Poly *p) {
	return PolyIsCoeff(p) && p->coeff == 0;
}

/**
 * Frees the allocated memory for a polynomial.
 * @param[in] p : polynomial
 */
void PolyDestroy(Poly *p);

/**
 * Frees the allocated memory for a monomial.
 * @param[in] m : monomial
 */
static inline void MonoDestroy(Mono *m) {
	PolyDestroy(&m->p);
}

/**
 * Clones a polynomial allocating a new block of memory for it.
 * @param[in] p : polynomial
 * @return cloned polynomial
 */
Poly PolyClone(const Poly *p);

/**
 * Clones a monomial allocating a new block of memory for it.
 * @param[in] m : monomial
 * @return cloned monomial
 */
static inline Mono MonoClone(const Mono *m) {
	return (Mono) {.p = PolyClone(&m->p), .exp = m->exp};
}

/**
 * Sums two polynomials.
 * @param[in] p : polynomial @f$p@f$
 * @param[in] q : polynomial @f$q@f$
 * @return @f$p + q@f$
 */
Poly PolyAdd(const Poly *p, const Poly *q);

/**
 * Sums an array of monomials and creates a polynomial
 * formed from the result. The created polynomial
 * is responsible for freeing the memory allocated
 * for the monomials in the array @p monos.
 * @param[in] count : number of monomials
 * @param[in] monos : array of monomials
 * @return polynomial being the sum of the monomials
 */
Poly PolyAddMonos(size_t count, const Mono monos[]);

/**
 * Sums an array of monomials and creates a polynomial
 * formed from the result. The polynomial is responsible for
 * freeing the allocated memory for the monomials and the array @p monos.
 * If @p count is equal to zero or if @p monos is a NULL pointer,
 * returns a zero polynomial.
 * @param[in] count : number of monomials
 * @param[in] monos : array of monomials
 * @return polynomial being the sum of the monomials
 */
Poly PolyOwnMonos(size_t count, Mono *monos);

/**
 * Sums an array of monomials and creates a polynomial formed
 * from the result. It doesn't modify the content of @p monos
 * or the array itself. If @p count is equal to zero or if @p monos is a NULL pointer,
 * returns a zero polynomial.
 * @param[in] count : number of monomials
 * @param[in] monos : array of monomials
 * @return polynomial being the sum of the monomials
 */
Poly PolyCloneMonos(size_t count, const Mono monos[]);

/**
 * Multiplies two polynomials.
 * @param[in] p : polynomial @f$p@f$
 * @param[in] q : polynomial @f$q@f$
 * @return @f$p * q@f$
 */
Poly PolyMul(const Poly *p, const Poly *q);

/**
 * Returns the opposite (negated) polynomial.
 * @param[in] p : polynomial @f$p@f$
 * @return @f$-p@f$
 */
Poly PolyNeg(const Poly *p);

/**
 * Subtracts one polynomial from another.
 * @param[in] p : polynomial @f$p@f$
 * @param[in] q : polynomial @f$q@f$
 * @return @f$p - q@f$
 */
Poly PolySub(const Poly *p, const Poly *q);

/**
 * Returns the degree of a polynomial in regard to the index of the variable
 * (-1 for a constant zero polynomial). The variables are indexed from 0.
 * The variable of index 0 represents the main variable of a polynomial.
 * Bigger indices refer to the polynomials being part of the coefficients
 * of a polynomial.
 * @param[in] p : polynomial
 * @param[in] var_idx : variable's index
 * @return degree of @p p in regard to the variable of index @p var_idx
 */
poly_exp_t PolyDegBy(const Poly *p, size_t var_idx);

/**
 * Returns the degree of a polynomial (-1 for constant polynomials equal to zero).
 * @param[in] p : polynomial
 * @return degree of @p p
 */
poly_exp_t PolyDeg(const Poly *p);

/**
 * Checks if two polynomials are equal.
 * @param[in] p : polynomial @f$p@f$
 * @param[in] q : polynomial @f$q@f$
 * @return @f$p = q@f$
 */
bool PolyIsEq(const Poly *p, const Poly *q);

/**
 * Computes the value of a polynomial at point @p x
 * by applying the argument to the main variable of the polynomial
 * (with index equal to 0). If the result is a polynomial, the indices
 * of its variables are lesser by one in comparison to the original polynomial.
 * Formally, for a polynomial @f$p(x_0, x_1, x_2, \ldots)@f$, the result is
 * the polynomial @f$p(x, x_0, x_1, \ldots)@f$.
 * @param[in] p : polynomial @f$p@f$
 * @param[in] x : the value of the argument @f$x@f$
 * @return @f$p(x, x_0, x_1, \ldots)@f$
 */
Poly PolyAt(const Poly *p, poly_coeff_t x);

/**
 * Composes a polynomial @f$p@f$ with @p k polynomial
 * from the array @p q and returns the result.
 * @param[in] p : polynomial
 * @param[in] k : number of polynomial in the array @p q
 * @param[in] q : array of polynomials
 * @return Composition of the polynomial @f$p@f$ with polynomials from
 * the array @p q.
 */
Poly PolyCompose(const Poly *p, size_t k, const Poly q[]);

#endif /* __POLY_H__ */

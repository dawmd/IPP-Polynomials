# <b>Multivariable polynomials library</b> #
## <b>Description</b> ##
The library implements multivariable polynomials in the integer domain (`long`). Every polynomial is defined recursively: it's either constant, represented by a single coefficient, or non-constant. In the latter case, it's represented as a sum:
<p align="center"><a href="https://www.codecogs.com/eqnedit.php?latex=\sum_{k=1}^{n}&space;p_kx_1^{\alpha_k}" target="_blank"><img src="https://latex.codecogs.com/gif.latex?\sum_{k=1}^{n}&space;p_kx_1^{\alpha_k}" title="\sum_{k=1}^{n} p_kx_1^{\alpha_k}" /></a>
</p>
where <a href="https://www.codecogs.com/eqnedit.php?latex=\inline&space;\alpha_k" target="_blank"><img src="https://latex.codecogs.com/gif.latex?\inline&space;\alpha_k" title="\alpha_k" /></a> are pairwise distinct non-negative integer numbers and <a href="https://www.codecogs.com/eqnedit.php?latex=\inline&space;p_k" target="_blank"><img src="https://latex.codecogs.com/gif.latex?\inline&space;p_k" title="p_k" /></a> are other polynomials. Assuming <a href="https://www.codecogs.com/eqnedit.php?latex=\inline&space;p_k" target="_blank"><img src="https://latex.codecogs.com/gif.latex?\inline&space;p_k" title="p_k" /></a> is a non-constant polynomial
<p align="center"><a href="https://www.codecogs.com/eqnedit.php?latex=p(x_1,&space;x_2,&space;\dots,&space;x_m)&space;=&space;\sum_{k=1}^{n}&space;p_k(x_2,&space;x_3,&space;\dots,&space;x_m)&space;\cdot&space;x_1^{\alpha_k}" target="_blank"><img src="https://latex.codecogs.com/gif.latex?p(x_1,&space;x_2,&space;\dots,&space;x_m)&space;=&space;\sum_{k=1}^{n}&space;p_k(x_2,&space;x_3,&space;\dots,&space;x_m)&space;\cdot&space;x_1^{\alpha_k}" title="p(x_1, x_2, \dots, x_m) = \sum_{k=1}^{n} p_k(x_2, x_3, \dots, x_m) \cdot x_1^{\alpha_k}" /></a>
</p>
In other words, polynomials being coefficients of the original polynomial have one fewer variables.

<br/>
<br/>

### <b>Operations</b> ###
The library provides basic operations that can be performed on multivariable polynomials:
* `CLONE` – clones a polynomial,
* `NEG` – returns a negated polynomial,
* `ADD` – adds two polynomials,
* `SUB` – subtracts two polynomials,
* `MUL` – multiplies two polynomials, 
* `IS_EQ` – checks if two polynomials are equal,
* `IS_COEFF` – checks if a polynomial is constant,
* `IS_ZERO` – checks if a polynomial is constant and equal to zero,
* `DEG` – checks the degree of a polynomial,
* `DEG_BY` – checks the degree of a polynomial in regard to one selected variable,
* `AT` – computes the value of a polynomial at a selected point,
* `COMPOSE` – composes a polynomial with others.

<br/>

### <b>Calculator</b> ###
The library also provides a console-based calculator. Aside from the operations described in the section above, it offers functions:
* `ZERO` – adds a zero polynomial onto the stack,
* `POP` – removes the polynomial from top of the stack.

<b>Possible errors</b>:
* `ERROR w WRONG COMMAND` – wrong command name,
* `ERROR w DEG BY WRONG VARIABLE` – no or incorrect parameter of function `DEG_BY`,
* `ERROR w AT WRONG VALUE` – no or incorrect parameter of function `AT`,
* `ERROR w COMPOSE WRONG PARAMETER` – no or incorrect parameter of function `COMPOSE`,
* `ERROR w STACK UNDERFLOW` – there are too few polynomials on the stack to perform an operation,
* `ERROR w WRONG POLY` – error while parsing a polynomial.

In all of these cases, `w` stands for the number of line the error occurred in.

<br />

#### <b>Polynomials syntax</b> ####
All polynomials are supposed to be provided in the following form:<br />
`(a_1,b_1)+(a_2,b_2)+(a_3,b_3)+...+(a_n,b_n)`<br />
Every term `a_k` stands for a coefficient (possibly another polynomial) of the `k`-th monomial, and every term `b_k` stands for the value of the corresponding exponent of the monomial. Note that no whitespaces are elligible. Every pair of consecutive monomials must also be separated by a `+` sign (`-` is not accepted).

<br />

#### <b>Technical aspects</b> ####
* The value of the argument of the operation `AT` is correct if and only if it's within `[-9223372036854775808, 9223372036854775807]`.
* The value of the exponent of a monomial is correct if and only if it's within `[0, 2147483647]`.
* The value of the argument of function `DEG_BY` is correct if and only if it's within `[0, 18446744073709551615]`.

All of those values must also be integer numbers.

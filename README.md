# Multivariable polynomials library #
## Description ##
<p>The library implements multivariable polynomials in the integer domain (`unsigned long`). Every polynomial is defined recurentially: it's either constant, represented by a single coefficient, or non-constant. In the latter case, it's represented as a sum of</p>
<p align="center"><a href="https://www.codecogs.com/eqnedit.php?latex=\sum_{k=1}^{n}&space;p_kx_1^{\alpha_k}" target="_blank"><img src="https://latex.codecogs.com/gif.latex?\sum_{k=1}^{n}&space;p_kx_1^{\alpha_k}" title="\sum_{k=1}^{n} p_kx_1^{\alpha_k}" /></a>
</p>
<p>
where <a href="https://www.codecogs.com/eqnedit.php?latex=\inline&space;\alpha_k" target="_blank"><img src="https://latex.codecogs.com/gif.latex?\inline&space;\alpha_k" title="\alpha_k" /></a> are pairwise distinct non-negative integer numbers and <a href="https://www.codecogs.com/eqnedit.php?latex=\inline&space;p_k" target="_blank"><img src="https://latex.codecogs.com/gif.latex?\inline&space;p_k" title="p_k" /></a> are other polynomials. Assuming <a href="https://www.codecogs.com/eqnedit.php?latex=\inline&space;p_k" target="_blank"><img src="https://latex.codecogs.com/gif.latex?\inline&space;p_k" title="p_k" /></a> is a non-constant polynomial
</p>
<p align="center"><a href="https://www.codecogs.com/eqnedit.php?latex=p(x_1,&space;x_2,&space;\dots,&space;x_m)&space;=&space;\sum_{k=1}^{n}&space;p_k(x_2,&space;x_3,&space;\dots,&space;x_m)&space;\cdot&space;x_1^{\alpha_k}" target="_blank"><img src="https://latex.codecogs.com/gif.latex?p(x_1,&space;x_2,&space;\dots,&space;x_m)&space;=&space;\sum_{k=1}^{n}&space;p_k(x_2,&space;x_3,&space;\dots,&space;x_m)&space;\cdot&space;x_1^{\alpha_k}" title="p(x_1, x_2, \dots, x_m) = \sum_{k=1}^{n} p_k(x_2, x_3, \dots, x_m) \cdot x_1^{\alpha_k}" /></a>
</p>
<p>
In other words, polynomials being coefficients of the original polynomial have one fewer variables.
</p>
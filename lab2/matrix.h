#ifndef MATRIX_H
#define MATRIX_H

#include <stddef.h>

#define IDX(a, n, i, j) ((a)[(size_t)(i) * ((size_t)(n) + 1) + (size_t)(j)])

double *matrix_generate(int n, unsigned seed);

double *matrix_copy(const double *a, int n);

void matrix_free(double *a);

double residual_max(const double *orig_a, int n, const double *x);

#endif

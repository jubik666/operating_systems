#include "matrix.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

double *matrix_generate(int n, unsigned seed){
    double *a = malloc((size_t)n * ((size_t)n + 1) * sizeof(double));
    if (!a) {
        fprintf(stderr, "matrix_generate: malloc failed\n");
        exit(1);
    }

    srand(seed);
    
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j <= n; ++j) {
            double v = ((double)rand() / RAND_MAX) * 20.0 - 10.0;
            IDX(a, n, i, j) = v;
        }
    }
    
    for (int i = 0; i < n; ++i) {
        double row_sum = 0.0;
        for (int j = 0; j < n; ++j) {
            if (j != i) row_sum += fabs(IDX(a, n, i, j));
        }
        double extra = ((double)rand() / RAND_MAX) * 5.0 + 1.0;
        double sign = (IDX(a, n, i, i) < 0) ? -1.0 : 1.0;
        IDX(a, n, i, i) = sign * (row_sum + extra);
    }
    return a;
}

double *matrix_copy(const double *a, int n) {
    size_t bytes = (size_t)n * ((size_t)n + 1) * sizeof(double);
    double *b = malloc(bytes);
    if (!b) {
        fprintf(stderr, "matrix_copy: malloc failed\n");
        exit(1);
    }
    for (size_t i = 0; i < bytes / sizeof(double); ++i) b[i] = a[i];
    return b;
}

void matrix_free(double *a) {
    free(a);
}

double residual_max(const double *orig_a, int n, const double *x) {
    double max_r = 0.0;
    for (int i = 0; i < n; ++i) {
        double lhs = 0.0;
        for (int j = 0; j < n; ++j) {
            lhs += IDX(orig_a, n, i, j) * x[j];
        }
        double r = fabs(lhs - IDX(orig_a, n, i, n));
        if (r > max_r) max_r = r;
    }
    return max_r;
}
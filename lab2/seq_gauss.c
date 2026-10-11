#include "seq_gauss.h"
#include "matrix.h"

void gauss_sequential(double *a, int n, double *x) {
    for (int k = 0; k < n - 1; ++k) {
        for (int i = k + 1; i < n; ++i) {
            double factor = IDX(a, n, i, k) / IDX(a, n, k, k);
            for (int j = k; j <= n; ++j) {
                IDX(a, n, i, j) -= factor * IDX(a, n, k, j);
            }
        }
    }
    for (int i = n - 1; i >= 0; --i) {
        double sum = IDX(a, n, i, n);
        for (int j = i + 1; j < n; ++j) {
            sum -= IDX(a, n, i, j) * x[j];
        }
        x[i] = sum / IDX(a, n, i, i);
    }
}
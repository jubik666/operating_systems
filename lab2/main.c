#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "matrix.h"
#include "seq_gauss.h"
#include "par_gauss.h"

int main(int argc, char *argv[]) {
    unsigned seed = (argc >= 4) ? (unsigned)atoi(argv[3]) : 42u;
    if (argc < 3) {
        fprintf(stderr, "not correct input\n");
        return 1;
    }
    int n = atoi(argv[1]);
    int max_threads = atoi(argv[2]);
    if (n < 2 || max_threads < 0) {
        fprintf(stderr, "not correct matrix or threads\n" );
        return 1;
    }
    double *orig = matrix_generate(n, seed);
    double *x = malloc((size_t)n * sizeof(double));
    struct timespec t0, t1;
    double *a_seq = matrix_copy(orig, n);
    clock_gettime(CLOCK_MONOTONIC, &t0);
    gauss_sequential(a_seq, n, x);
    clock_gettime(CLOCK_MONOTONIC, &t1);
    double seq_ms = (double)(t1.tv_sec - t0.tv_sec) * 1000.0 + (double)(t1.tv_nsec - t0.tv_nsec) / 1000000.0;
    double seq_res = residual_max(orig, n, x);
    printf("sequential: n=%d time_ms=%.3f residual=%.3e\n", n, seq_ms, seq_res);
    if (max_threads > 0) {
        double *a_par = matrix_copy(orig, n);

        clock_gettime(CLOCK_MONOTONIC, &t0);
        gauss_parallel(a_par, n, x, max_threads);
        clock_gettime(CLOCK_MONOTONIC, &t1);

        double par_ms = (double)(t1.tv_sec - t0.tv_sec) * 1000.0 + (double)(t1.tv_nsec - t0.tv_nsec) / 1000000.0;
        double par_res = residual_max(orig, n, x);

        double speedup = seq_ms / par_ms;
        double efficiency = speedup / max_threads;

        printf("parallel: n=%d threads=%d time_ms=%.3f residual=%.3e speedup=%.3f efficiency=%.3f\n", n, max_threads, par_ms, par_res, speedup, efficiency);

        matrix_free(a_par);
    }
    free(x);
    matrix_free(orig);
    matrix_free(a_seq);
    return 0;
}
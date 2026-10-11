#include "par_gauss.h"
#include "matrix.h"
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct {
    double *a;
    int n;
    int k;
    int start;
    int end;
} parka;

static void *worker(void *arg) {
    parka *t = (parka *)arg;
    double *a = t->a;
    int n = t->n;
    int k = t->k;
    for (int i = t->start; i < t->end; ++i) {
        double factor = IDX(a, n, i, k) / IDX(a, n, k, k);
        for (int j = k; j <= n; ++j) {
            IDX(a, n, i, j) -= factor * IDX(a, n, k, j);
        }
    }
    return NULL;
}

void gauss_parallel(double *a, int n, double *x, int max_threads) {
    for (int k = 0; k <= n - 2; ++k) {
        int remaining = n - 1 - k;
        int t_count = (max_threads < remaining) ? max_threads : remaining;
        pthread_t *threads = malloc((size_t)t_count * sizeof(pthread_t));
        parka *tasks = malloc((size_t)t_count * sizeof(parka));
        int base = remaining / t_count;
        int extra = remaining % t_count;
        for (int t = 0; t < t_count; ++ t) {
            int my_count = base + (t < extra ? 1 : 0);
            int offset = t * base + (t < extra ? t : extra);
            int start = k + 1 + offset;
            int end = start + my_count;
            tasks[t].a = a;
            tasks[t].n = n;
            tasks[t].k = k;
            tasks[t].start = start;
            tasks[t].end = end;
            pthread_create(&threads[t], NULL, worker, &tasks[t]);
        }
        for (int t = 0; t < t_count; ++t) {
            pthread_join(threads[t], NULL);
        }
        free(threads);
        free(tasks);
    }
    for (int i = n - 1; i >= 0; i--) {
        double sum = IDX(a, n, i, n);
        for (int j = i + 1; j < n; j++) {
            sum -= IDX(a, n, i, j) * x[j];
        }
        x[i] = sum / IDX(a, n, i, i);
    }
}
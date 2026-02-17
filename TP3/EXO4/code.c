#include <stdio.h>
#include <stdlib.h>
#ifdef _OPENMP
#include <omp.h>
#endif

int main(){
    int m = 500, n = 500;
    int i, j, k;

    double *a = (double*)malloc(m * n * sizeof(double));
    double *b = (double*)malloc(n * m * sizeof(double));
    double *c = (double*)malloc(m * m * sizeof(double));

    double start_time, end_time;

    for (i = 0; i < m; i++) {
        for (j = 0; j < n; j++) {
            a[i * n + j] = (i + 1) + (j + 1);
        }
    }

    for (i = 0; i < n; i++) {
        for (j = 0; j < m; j++) {
            b[i * m + j] = (i + 1) - (j + 1);
        }
    }

    for (i = 0; i < m; i++) {
        for (j = 0; j < m; j++) {
            c[i * m + j] = 0;
        }
    }

    start_time = omp_get_wtime();

    #pragma omp parallel for collapse(2) private(i, j, k) shared(a, b, c) schedule(runtime)
    for (i = 0; i < m; i++) {
        for (j = 0; j < m; j++) {
            for (k = 0; k < n; k++) {
                c[i * m + j] += a[i * n + k] * b[k * m + j];
            }
        }
    }

    end_time = omp_get_wtime();

    printf("Time: %f seconds\n", end_time - start_time);

    free(a);
    free(b);
    free(c);

    return 0;
}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <math.h>
#include <sys/time.h>
#ifdef _OPENMP
#include <omp.h>
#endif

#ifndef VAL_N
#define VAL_N 120
#endif

#ifndef VAL_D
#define VAL_D 80
#endif

#define MAX_ITER 1000
#define TOLERANCE 1e-6

void random_number(double* array, int size) {
    for (int i = 0; i < size; i++) {
        array[i] = (double)rand() / (double)(RAND_MAX - 1);
    }
}

int main() {
    int n = VAL_N, diag = VAL_D;
    int i, j, iteration = 0;
    double norme;

    double *a = (double*)malloc(n * n * sizeof(double));
    double *x = (double*)malloc(n * sizeof(double));
    double *x_courant = (double*)malloc(n * sizeof(double));
    double *b = (double*)malloc(n * sizeof(double));

    if (!a || !x || !x_courant || !b) {
        fprintf(stderr, "Memory Allocation Failed!\n");
        exit(EXIT_FAILURE);
    }

    struct timeval t_elapsed_0, t_elapsed_1;
    double t_elapsed;
    double t_cpu_0, t_cpu_1, t_cpu;

    srand(421);
    random_number(a, n * n);
    random_number(b, n);

    // Make matrix diagonally dominant for convergence
    for (i = 0; i < n; i++) {
        a[i * n + i] += diag;
    }

    // Initialize solution vector
    for (i = 0; i < n; i++) {
        x[i] = 1.0;
    }

    t_cpu_0 = omp_get_wtime();
    gettimeofday(&t_elapsed_0, NULL);

    // Gauss-Seidel iteration
    while (iteration < MAX_ITER) {
        iteration++;

        // Parallel Gauss-Seidel using coloring or red-black scheme
        #pragma omp parallel for private(i, j) shared(a, b, x, x_courant) schedule(static)
        for (i = 0; i < n; i++) {
            double sum = 0.0;
            
            // Lower triangular part (use updated values)
            for (j = 0; j < i; j++) {
                sum += a[i * n + j] * x_courant[j];
            }
            
            // Upper triangular part (use old values)
            for (j = i + 1; j < n; j++) {
                sum += a[i * n + j] * x[j];
            }
            
            x_courant[i] = (b[i] - sum) / a[i * n + i];
        }

        // Compute convergence criterion (infinity norm)
        double absmax = 0.0;
        #pragma omp parallel for reduction(max:absmax)
        for (i = 0; i < n; i++) {
            double curr = fabs(x[i] - x_courant[i]);
            if (curr > absmax) {
                absmax = curr;
            }
        }
        norme = absmax;

        // Check convergence
        if (norme <= TOLERANCE) break;

        // Copy new solution to x
        memcpy(x, x_courant, n * sizeof(double));
    }

    gettimeofday(&t_elapsed_1, NULL);
    t_elapsed = (t_elapsed_1.tv_sec - t_elapsed_0.tv_sec) + 
                (t_elapsed_1.tv_usec - t_elapsed_0.tv_usec) / 1e6;

    t_cpu_1 = omp_get_wtime();
    t_cpu = t_cpu_1 - t_cpu_0;

    fprintf(stdout, "\nSystem size: %5d\nIterations: %4d\nNorme: %10.3E\n", 
            n, iteration, norme);
    fprintf(stdout, "Elapsed time: %10.3E sec\nCPU time: %10.3E sec\n", 
            t_elapsed, t_cpu);

    free(a);
    free(x);
    free(x_courant);
    free(b);

    return EXIT_SUCCESS;
}
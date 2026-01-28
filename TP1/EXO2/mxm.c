#include "stdio.h"
#include "stdlib.h"
#include "time.h"
#include "string.h"

#define N 1024

void matrix_mul_ijk(double **a, double **b, double **c, int n){
    for (int i = 0; i < n; i++){
        for (int j = 0; j < n; j++){
            for (int k = 0; k < n; k++){
                c[i][j] += a[i][k] * b[k][j];
            }
        }
    }
}


void matrix_mul_ikj(double **a, double **b, double **c, int n){
    for (int i = 0; i < n; i++){
        for (int k = 0; k < n; k++){
            double r = a[i][k];
            for (int j = 0; j < n; j++){
                c[i][j] += r * b[k][j];
            }
        }
    }
}


double** allocate_matrix(int n){
    double **matrix = (double**)malloc(n * sizeof(double*));
    for (int i = 0; i < n; i++){
        matrix[i] = (double*)calloc(n, sizeof(double));
    }
    return matrix;
}

void free_matrix(double **matrix, int n){
    for (int i = 0; i < n; i++){
        free(matrix[i]);
    }
    free(matrix);
}

void init_matrix(double **matrix, int n){
    for (int i = 0; i < n; i++){
        for (int j = 0; j < n; j++){
            matrix[i][j] = (double)rand() / RAND_MAX;
        }
    }
}

// Time Measure (msec)
double measure_time(void (*func)(double**, double**, double**, int),
                    double **a, double **b, double **c, int n){
    struct timespec start, end;
    for (int i = 0; i < n; i++){
        memset(c[i], 0, n * sizeof(double));
    }

    clock_gettime(CLOCK_MONOTONIC, &start);
    func(a, b, c, n);
    clock_gettime(CLOCK_MONOTONIC, &end);

    double time_ms = (end.tv_sec - start.tv_sec) * 1000.0 +
                     (end.tv_nsec - start.tv_nsec) / 1000000.0;

    return time_ms;
}

int main() {
    int sizes[] = {256, 512, 1024};
    int num_sizes = sizeof(sizes) / sizeof(sizes[0]);

    printf("Matrix Multiplication Performance Comparison\n");
    printf("============================================\n\n");

    for (int s = 0; s < num_sizes; s++) {
        int n = sizes[s];
        printf("Matrix size: %d x %d\n", n, n);

        double **a = allocate_matrix(n);
        double **b = allocate_matrix(n);
        double **c = allocate_matrix(n);

        srand(42);
        init_matrix(a, n);
        init_matrix(b, n);

        double time_ijk = measure_time(matrix_mul_ijk, a, b, c, n);

        double ops = 2.0 * n * n * n;
        double gflops_ijk = (ops / 1e9) / (time_ijk / 1000.0);

        double memory_accessed_gb = (3.0 * n * n * n * sizeof(double)) / 1e9;
        double bandwidth_ijk = memory_accessed_gb / (time_ijk / 1000.0);

        printf("  ijk order:\n");
        printf("    Time: %.2f ms\n", time_ijk);
        printf("    Performance: %.2f GFLOPS\n", gflops_ijk);
        printf("    Bandwidth: %.2f GB/s\n", bandwidth_ijk);


        double time_ikj = measure_time(matrix_mul_ikj, a, b, c, n);
        double gflops_ikj = (ops / 1e9) / (time_ikj / 1000.0);
        double bandwidth_ikj = memory_accessed_gb / (time_ikj / 1000.0);

        printf("  ikj order:\n");
        printf("    Time: %.2f ms\n", time_ikj);
        printf("    Performance: %.2f GFLOPS\n", gflops_ikj);
        printf("    Bandwidth: %.2f GB/s\n", bandwidth_ikj);

        double speedup = time_ijk / time_ikj;
        printf("    Speedup: %.2fx\n\n", speedup);

        free_matrix(a, n);
        free_matrix(b, n);
        free_matrix(c, n);
    }

    printf("\n Explanation:\n");
    printf("- ijk order: Poor cache locality for matrix b (column access)\n");
    printf("- ikj order: Better cache locality (row-wise access for both a and b)\n");
    printf("- Speedup increases with matrix size due to cache effects\n");

    return 0;
}
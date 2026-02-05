#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 512  // Matrix size

/* ===== Generate noise ===== */
void generate_noise(double *noise) {
    noise[0] = 1.0;
    for (int i = 1; i < N; i++) {
        noise[i] = noise[i-1] * 1.0000001;
    }
}

/* ===== Matrices Initialization ===== */
void init_matrix(double *M) {
    for (int i = 0; i < N*N; i++) {
        M[i] = (double)(i % 100) * 0.01;
    }
}

/* ===== Matrix Multiplication ===== */
void matmul(double *A, double *B, double *C, double *noise) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            double sum = noise[i];
            for (int k = 0; k < N; k++) {
                sum += A[i*N + k] * B[k*N + j];
            }
            C[i*N + j] = sum;
        }
    }
}

int main() {
    clock_t start_total, end_total;
    clock_t start, end;
    
    printf("==========================================\n");
    printf("Exercise 4: Matrix Multiplication Analysis\n");
    printf("N = %d (Matrix size: %dx%d)\n", N, N, N);
    printf("==========================================\n\n");
    
    start_total = clock();
    
    double *A = malloc(N*N * sizeof(double));
    double *B = malloc(N*N * sizeof(double));
    double *C = malloc(N*N * sizeof(double));
    double *noise = malloc(N * sizeof(double));
    
    if (!A || !B || !C || !noise) {
        fprintf(stderr, "Memory allocation failed!\n");
        return 1;
    }
    
    // Time each function
    start = clock();
    generate_noise(noise);
    end = clock();
    double time_noise = (double)(end - start) / CLOCKS_PER_SEC;
    printf("generate_noise:    %.6f seconds\n", time_noise);
    
    start = clock();
    init_matrix(A);
    end = clock();
    double time_init_a = (double)(end - start) / CLOCKS_PER_SEC;
    printf("init_matrix (A):   %.6f seconds\n", time_init_a);
    
    start = clock();
    init_matrix(B);
    end = clock();
    double time_init_b = (double)(end - start) / CLOCKS_PER_SEC;
    printf("init_matrix (B):   %.6f seconds\n", time_init_b);
    
    start = clock();
    matmul(A, B, C, noise);
    end = clock();
    double time_matmul = (double)(end - start) / CLOCKS_PER_SEC;
    printf("matmul:            %.6f seconds\n", time_matmul);
    
    end_total = clock();
    double time_total = (double)(end_total - start_total) / CLOCKS_PER_SEC;
    
    printf("\n==========================================\n");
    printf("Results:\n");
    printf("C[0] = %.6f\n", C[0]);
    printf("Total time: %.6f seconds\n", time_total);
    
    // Calculate percentages
    double total_measured = time_noise + time_init_a + time_init_b + time_matmul;
    printf("\n==========================================\n");
    printf("Time Distribution:\n");
    printf("generate_noise:    %.4f%% (%.6fs)\n", (time_noise / total_measured) * 100, time_noise);
    printf("init_matrix (A):   %.4f%% (%.6fs)\n", (time_init_a / total_measured) * 100, time_init_a);
    printf("init_matrix (B):   %.4f%% (%.6fs)\n", (time_init_b / total_measured) * 100, time_init_b);
    printf("matmul:            %.4f%% (%.6fs)\n", (time_matmul / total_measured) * 100, time_matmul);
    
    printf("\n==========================================\n");
    printf("Sequential Fraction Analysis:\n");
    printf("Sequential (generate_noise): %.4f%%\n", (time_noise / total_measured) * 100);
    printf("Parallel (others):           %.4f%%\n", ((total_measured - time_noise) / total_measured) * 100);
    printf("fs (sequential fraction) = %.8f\n", time_noise / total_measured);
    
    printf("\n==========================================\n");
    printf("Operation Counts:\n");
    printf("generate_noise: %d operations (O(N))\n", N);
    printf("init_matrix(A): %d operations (O(N²))\n", N*N);
    printf("init_matrix(B): %d operations (O(N²))\n", N*N);
    long long matmul_ops = (long long)N * N * N;
    printf("matmul:         %lld operations (O(N³))\n", matmul_ops);
    
    printf("\n==========================================\n");
    printf("Theoretical Sequential Fraction:\n");
    long long total_ops = N + 2LL*N*N + (long long)N*N*N;
    printf("fs_theoretical = %d / %lld = %.8f\n", N, total_ops, (double)N / total_ops);
    printf("==========================================\n");
    
    free(A);
    free(B);
    free(C);
    free(noise);
    return 0;
}
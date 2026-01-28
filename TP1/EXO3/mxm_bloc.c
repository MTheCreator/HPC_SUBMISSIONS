#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define N 1024  // Matrix size

// Standard matrix multiplication (for comparison)
void matrix_mult_standard(double **a, double **b, double **c, int n) {
    for (int i = 0; i < n; i++) {
        for (int k = 0; k < n; k++) {
            double r = a[i][k];
            for (int j = 0; j < n; j++) {
                c[i][j] += r * b[k][j];
            }
        }
    }
}

// Blocked matrix multiplication
void matrix_mult_blocked(double **a, double **b, double **c, int n, int block_size) {
    // Outer loops iterate over blocks
    for (int ii = 0; ii < n; ii += block_size) {
        for (int jj = 0; jj < n; jj += block_size) {
            for (int kk = 0; kk < n; kk += block_size) {
                
                // Inner loops process elements within each block
                for (int i = ii; i < ii + block_size && i < n; i++) {
                    for (int k = kk; k < kk + block_size && k < n; k++) {
                        double r = a[i][k];
                        for (int j = jj; j < jj + block_size && j < n; j++) {
                            c[i][j] += r * b[k][j];
                        }
                    }
                }
            }
        }
    }
}

// Allocate 2D matrix
double** allocate_matrix(int n) {
    double **matrix = (double**)malloc(n * sizeof(double*));
    for (int i = 0; i < n; i++) {
        matrix[i] = (double*)calloc(n, sizeof(double));
    }
    return matrix;
}

// Free 2D matrix
void free_matrix(double **matrix, int n) {
    for (int i = 0; i < n; i++) {
        free(matrix[i]);
    }
    free(matrix);
}

// Initialize matrix with random values
void initialize_matrix(double **matrix, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            matrix[i][j] = (double)rand() / RAND_MAX;
        }
    }
}

// Reset matrix to zero
void reset_matrix(double **matrix, int n) {
    for (int i = 0; i < n; i++) {
        memset(matrix[i], 0, n * sizeof(double));
    }
}

// Measure execution time in milliseconds
double measure_time_standard(double **a, double **b, double **c, int n) {
    struct timespec start, end;
    
    reset_matrix(c, n);
    
    clock_gettime(CLOCK_MONOTONIC, &start);
    matrix_mult_standard(a, b, c, n);
    clock_gettime(CLOCK_MONOTONIC, &end);
    
    double time_ms = (end.tv_sec - start.tv_sec) * 1000.0 +
                     (end.tv_nsec - start.tv_nsec) / 1000000.0;
    return time_ms;
}

// Measure execution time for blocked version
double measure_time_blocked(double **a, double **b, double **c, int n, int block_size) {
    struct timespec start, end;
    
    reset_matrix(c, n);
    
    clock_gettime(CLOCK_MONOTONIC, &start);
    matrix_mult_blocked(a, b, c, n, block_size);
    clock_gettime(CLOCK_MONOTONIC, &end);
    
    double time_ms = (end.tv_sec - start.tv_sec) * 1000.0 +
                     (end.tv_nsec - start.tv_nsec) / 1000000.0;
    return time_ms;
}

int main() {
    int n = N;
    int block_sizes[] = {16, 32, 64, 128, 256};
    int num_blocks = sizeof(block_sizes) / sizeof(block_sizes[0]);
    
    printf("========================================================\n");
    printf("Blocked Matrix Multiplication Performance Analysis\n");
    printf("========================================================\n");
    printf("Matrix size: %d x %d\n\n", n, n);
    
    // Allocate matrices
    double **a = allocate_matrix(n);
    double **b = allocate_matrix(n);
    double **c = allocate_matrix(n);
    
    // Initialize matrices
    srand(42);
    initialize_matrix(a, n);
    initialize_matrix(b, n);
    
    // Calculate theoretical values
    double ops = 2.0 * n * n * n;  // 2*n^3 operations
    double memory_accessed_gb = (3.0 * n * n * n * sizeof(double)) / 1e9;
    
    printf("Theoretical Operations: %.2f GFLOP\n", ops / 1e9);
    printf("Approximate Memory Accessed: %.2f GB\n\n", memory_accessed_gb);
    
    // Measure standard version (baseline)
    printf("BASELINE (No blocking):\n");
    printf("--------------------------------------------------------\n");
    double time_standard = measure_time_standard(a, b, c, n);
    double gflops_standard = (ops / 1e9) / (time_standard / 1000.0);
    double bandwidth_standard = memory_accessed_gb / (time_standard / 1000.0);
    
    printf("  Time:       %8.2f ms\n", time_standard);
    printf("  Performance: %7.2f GFLOPS\n", gflops_standard);
    printf("  Bandwidth:   %7.2f GB/s\n", bandwidth_standard);
    printf("  Speedup:     %7.2fx (baseline)\n\n", 1.0);
    
    // Test different block sizes
    printf("BLOCKED VERSIONS:\n");
    printf("--------------------------------------------------------\n");
    printf("Block Size | Time (ms) | GFLOPS | Bandwidth | Speedup\n");
    printf("--------------------------------------------------------\n");
    
    double best_time = time_standard;
    int best_block_size = 0;
    
    for (int i = 0; i < num_blocks; i++) {
        int block_size = block_sizes[i];
        
        double time_blocked = measure_time_blocked(a, b, c, n, block_size);
        double gflops_blocked = (ops / 1e9) / (time_blocked / 1000.0);
        double bandwidth_blocked = memory_accessed_gb / (time_blocked / 1000.0);
        double speedup = time_standard / time_blocked;
        
        printf("%6d     | %9.2f | %6.2f | %9.2f | %7.2fx",
               block_size, time_blocked, gflops_blocked, bandwidth_blocked, speedup);
        
        if (time_blocked < best_time) {
            best_time = time_blocked;
            best_block_size = block_size;
            printf(" <- BEST");
        }
        printf("\n");
    }
    
    printf("--------------------------------------------------------\n\n");
    
    // Analysis
    printf("========================================================\n");
    printf("ANALYSIS:\n");
    printf("========================================================\n");
    printf("Optimal Block Size: %d x %d\n", best_block_size, best_block_size);
    printf("Maximum Speedup: %.2fx\n\n", time_standard / best_time);
    
    printf("WHY IS THIS THE OPTIMAL BLOCK SIZE?\n");
    printf("--------------------------------------------------------\n");
    
    // Calculate cache requirements
    double block_memory_kb = (3.0 * best_block_size * best_block_size * sizeof(double)) / 1024.0;
    
    printf("1. Cache Considerations:\n");
    printf("   - Each block requires ~%.2f KB for 3 blocks (A, B, C)\n", block_memory_kb);
    printf("   - Fits well in L1/L2 cache (typically 32-256 KB per core)\n");
    printf("   - Maximizes data reuse before eviction\n\n");
    
    printf("2. Block Size Effects:\n");
    printf("   - Too small (16-32): Overhead from extra loop iterations\n");
    printf("   - Too large (256+): Exceeds cache capacity, more misses\n");
    printf("   - Optimal (%d): Balance between cache utilization and overhead\n\n", best_block_size);
    
    printf("3. Cache Hierarchy (typical Mac M-series):\n");
    printf("   - L1 Cache: 128-192 KB (per core)\n");
    printf("   - L2 Cache: 12-24 MB (shared)\n");
    printf("   - Block size %d fits in L1 with room for other data\n\n", best_block_size);
    
    printf("4. Performance Characteristics:\n");
    printf("   - Blocks fit entirely in fast cache levels\n");
    printf("   - Minimizes main memory accesses\n");
    printf("   - Reduces TLB misses\n");
    printf("   - Improves spatial and temporal locality\n\n");
    
    // Free matrices
    free_matrix(a, n);
    free_matrix(b, n);
    free_matrix(c, n);
    
    return 0;
}
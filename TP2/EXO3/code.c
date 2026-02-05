#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 100000000

void add_noise(double *a) {
    a[0] = 1.0;
    for (int i = 1; i < N; i++) {
        a[i] = a[i-1] * 1.0000001;
    }
}

/* ===== Initialization ===== */
void init_b(double *b) {
    for (int i = 0; i < N; i++) {
        b[i] = i * 0.5;
    }
}

/* ===== Compute addition ===== */
void compute_addition(double *a, double *b, double *c) {
    for (int i = 0; i < N; i++) {
        c[i] = a[i] + b[i];
    }
}

/* ===== Reduction ===== */
double reduction(double *c) {
    double sum = 0.0;
    for (int i = 0; i < N; i++) {
        sum += c[i];
    }
    return sum;
}

int main() {
    clock_t start_total, end_total;
    clock_t start, end;
    
    printf("==========================================\n");
    printf("Exercise 3: Parallel Analysis\n");
    printf("N = %d\n", N);
    printf("==========================================\n\n");
    
    start_total = clock();
    
    double *a = malloc(N * sizeof(double));
    double *b = malloc(N * sizeof(double));
    double *c = malloc(N * sizeof(double));
    
    if (!a || !b || !c) {
        fprintf(stderr, "Memory allocation failed!\n");
        return 1;
    }
    
    // Time each function
    start = clock();
    add_noise(a);
    end = clock();
    double time_noise = (double)(end - start) / CLOCKS_PER_SEC;
    printf("add_noise:         %.4f seconds\n", time_noise);
    
    start = clock();
    init_b(b);
    end = clock();
    double time_init = (double)(end - start) / CLOCKS_PER_SEC;
    printf("init_b:            %.4f seconds\n", time_init);
    
    start = clock();
    compute_addition(a, b, c);
    end = clock();
    double time_add = (double)(end - start) / CLOCKS_PER_SEC;
    printf("compute_addition:  %.4f seconds\n", time_add);
    
    start = clock();
    double sum = reduction(c);
    end = clock();
    double time_red = (double)(end - start) / CLOCKS_PER_SEC;
    printf("reduction:         %.4f seconds\n", time_red);
    
    end_total = clock();
    double time_total = (double)(end_total - start_total) / CLOCKS_PER_SEC;
    
    printf("\n==========================================\n");
    printf("Results:\n");
    printf("Sum = %.6f\n", sum);
    printf("Total time: %.4f seconds\n", time_total);
    
    // Calculate percentages
    double total_measured = time_noise + time_init + time_add + time_red;
    printf("\n==========================================\n");
    printf("Time Distribution:\n");
    printf("add_noise:         %.2f%%\n", (time_noise / total_measured) * 100);
    printf("init_b:            %.2f%%\n", (time_init / total_measured) * 100);
    printf("compute_addition:  %.2f%%\n", (time_add / total_measured) * 100);
    printf("reduction:         %.2f%%\n", (time_red / total_measured) * 100);
    
    printf("\n==========================================\n");
    printf("Sequential Fraction Analysis:\n");
    printf("Sequential (add_noise):  %.2f%%\n", (time_noise / total_measured) * 100);
    printf("Parallel (others):       %.2f%%\n", ((total_measured - time_noise) / total_measured) * 100);
    printf("fs (sequential fraction) = %.4f\n", time_noise / total_measured);
    printf("==========================================\n");
    
    free(a);
    free(b);
    free(c);
    return 0;
}
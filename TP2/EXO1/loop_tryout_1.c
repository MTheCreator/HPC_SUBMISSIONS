#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 1000000
#define TRIALS 10

// ============================================================================
// DOUBLE BENCHMARKS
// ============================================================================
void benchmark_double() {
    double *a = malloc(N * sizeof(double));
    for (int i = 0; i < N; i++) a[i] = 1.0;
    
    printf("\n=== DOUBLE (8 bytes) ===\n");
    printf("Unroll\tTime(ms)\tSpeedup\n");
    printf("------------------------------------\n");
    
    double baseline = 0.0;
    
    // U=1
    {
        double total = 0.0;
        for (int t = 0; t < TRIALS; t++) {
            double sum = 0.0;
            clock_t start = clock();
            for (int i = 0; i < N; i++) sum += a[i];
            clock_t end = clock();
            total += ((double)(end - start) / CLOCKS_PER_SEC) * 1000;
            if (sum < 0) printf("");  // Prevent optimization
        }
        baseline = total / TRIALS;
        printf("1\t%.3f\t\t1.00x\n", baseline);
    }
    
    // U=2
    {
        double total = 0.0;
        for (int t = 0; t < TRIALS; t++) {
            double sum = 0.0;
            clock_t start = clock();
            int i;
            for (i = 0; i < N-1; i += 2) sum += a[i] + a[i+1];
            for (; i < N; i++) sum += a[i];
            clock_t end = clock();
            total += ((double)(end - start) / CLOCKS_PER_SEC) * 1000;
            if (sum < 0) printf("");
        }
        double avg = total / TRIALS;
        printf("2\t%.3f\t\t%.2fx\n", avg, baseline/avg);
    }
    
    // U=4
    {
        double total = 0.0;
        for (int t = 0; t < TRIALS; t++) {
            double sum = 0.0;
            clock_t start = clock();
            int i;
            for (i = 0; i < N-3; i += 4) 
                sum += a[i] + a[i+1] + a[i+2] + a[i+3];
            for (; i < N; i++) sum += a[i];
            clock_t end = clock();
            total += ((double)(end - start) / CLOCKS_PER_SEC) * 1000;
            if (sum < 0) printf("");
        }
        double avg = total / TRIALS;
        printf("4\t%.3f\t\t%.2fx\n", avg, baseline/avg);
    }
    
    // U=8
    {
        double total = 0.0;
        for (int t = 0; t < TRIALS; t++) {
            double sum = 0.0;
            clock_t start = clock();
            int i;
            for (i = 0; i < N-7; i += 8) 
                sum += a[i] + a[i+1] + a[i+2] + a[i+3] +
                       a[i+4] + a[i+5] + a[i+6] + a[i+7];
            for (; i < N; i++) sum += a[i];
            clock_t end = clock();
            total += ((double)(end - start) / CLOCKS_PER_SEC) * 1000;
            if (sum < 0) printf("");
        }
        double avg = total / TRIALS;
        printf("8\t%.3f\t\t%.2fx\n", avg, baseline/avg);
    }
    
    // U=16
    {
        double total = 0.0;
        for (int t = 0; t < TRIALS; t++) {
            double sum = 0.0;
            clock_t start = clock();
            int i;
            for (i = 0; i < N-15; i += 16) 
                sum += a[i] + a[i+1] + a[i+2] + a[i+3] +
                       a[i+4] + a[i+5] + a[i+6] + a[i+7] +
                       a[i+8] + a[i+9] + a[i+10] + a[i+11] +
                       a[i+12] + a[i+13] + a[i+14] + a[i+15];
            for (; i < N; i++) sum += a[i];
            clock_t end = clock();
            total += ((double)(end - start) / CLOCKS_PER_SEC) * 1000;
            if (sum < 0) printf("");
        }
        double avg = total / TRIALS;
        printf("16\t%.3f\t\t%.2fx\n", avg, baseline/avg);
    }
    
    // U=32
    {
        double total = 0.0;
        for (int t = 0; t < TRIALS; t++) {
            double sum = 0.0;
            clock_t start = clock();
            int i;
            for (i = 0; i < N-31; i += 32) 
                sum += a[i] + a[i+1] + a[i+2] + a[i+3] +
                       a[i+4] + a[i+5] + a[i+6] + a[i+7] +
                       a[i+8] + a[i+9] + a[i+10] + a[i+11] +
                       a[i+12] + a[i+13] + a[i+14] + a[i+15] +
                       a[i+16] + a[i+17] + a[i+18] + a[i+19] +
                       a[i+20] + a[i+21] + a[i+22] + a[i+23] +
                       a[i+24] + a[i+25] + a[i+26] + a[i+27] +
                       a[i+28] + a[i+29] + a[i+30] + a[i+31];
            for (; i < N; i++) sum += a[i];
            clock_t end = clock();
            total += ((double)(end - start) / CLOCKS_PER_SEC) * 1000;
            if (sum < 0) printf("");
        }
        double avg = total / TRIALS;
        printf("32\t%.3f\t\t%.2fx\n", avg, baseline/avg);
    }
    
    free(a);
}

// ============================================================================
// FLOAT BENCHMARKS
// ============================================================================
void benchmark_float() {
    float *a = malloc(N * sizeof(float));
    for (int i = 0; i < N; i++) a[i] = 1.0f;
    
    printf("\n=== FLOAT (4 bytes) ===\n");
    printf("Unroll\tTime(ms)\tSpeedup\n");
    printf("------------------------------------\n");
    
    double baseline = 0.0;
    
    // U=1
    {
        double total = 0.0;
        for (int t = 0; t < TRIALS; t++) {
            float sum = 0.0f;
            clock_t start = clock();
            for (int i = 0; i < N; i++) sum += a[i];
            clock_t end = clock();
            total += ((double)(end - start) / CLOCKS_PER_SEC) * 1000;
            if (sum < 0) printf("");
        }
        baseline = total / TRIALS;
        printf("1\t%.3f\t\t1.00x\n", baseline);
    }
    
    // U=2
    {
        double total = 0.0;
        for (int t = 0; t < TRIALS; t++) {
            float sum = 0.0f;
            clock_t start = clock();
            int i;
            for (i = 0; i < N-1; i += 2) sum += a[i] + a[i+1];
            for (; i < N; i++) sum += a[i];
            clock_t end = clock();
            total += ((double)(end - start) / CLOCKS_PER_SEC) * 1000;
            if (sum < 0) printf("");
        }
        double avg = total / TRIALS;
        printf("2\t%.3f\t\t%.2fx\n", avg, baseline/avg);
    }
    
    // U=4
    {
        double total = 0.0;
        for (int t = 0; t < TRIALS; t++) {
            float sum = 0.0f;
            clock_t start = clock();
            int i;
            for (i = 0; i < N-3; i += 4) 
                sum += a[i] + a[i+1] + a[i+2] + a[i+3];
            for (; i < N; i++) sum += a[i];
            clock_t end = clock();
            total += ((double)(end - start) / CLOCKS_PER_SEC) * 1000;
            if (sum < 0) printf("");
        }
        double avg = total / TRIALS;
        printf("4\t%.3f\t\t%.2fx\n", avg, baseline/avg);
    }
    
    // U=8
    {
        double total = 0.0;
        for (int t = 0; t < TRIALS; t++) {
            float sum = 0.0f;
            clock_t start = clock();
            int i;
            for (i = 0; i < N-7; i += 8) 
                sum += a[i] + a[i+1] + a[i+2] + a[i+3] +
                       a[i+4] + a[i+5] + a[i+6] + a[i+7];
            for (; i < N; i++) sum += a[i];
            clock_t end = clock();
            total += ((double)(end - start) / CLOCKS_PER_SEC) * 1000;
            if (sum < 0) printf("");
        }
        double avg = total / TRIALS;
        printf("8\t%.3f\t\t%.2fx\n", avg, baseline/avg);
    }
    
    // U=16
    {
        double total = 0.0;
        for (int t = 0; t < TRIALS; t++) {
            float sum = 0.0f;
            clock_t start = clock();
            int i;
            for (i = 0; i < N-15; i += 16) 
                sum += a[i] + a[i+1] + a[i+2] + a[i+3] +
                       a[i+4] + a[i+5] + a[i+6] + a[i+7] +
                       a[i+8] + a[i+9] + a[i+10] + a[i+11] +
                       a[i+12] + a[i+13] + a[i+14] + a[i+15];
            for (; i < N; i++) sum += a[i];
            clock_t end = clock();
            total += ((double)(end - start) / CLOCKS_PER_SEC) * 1000;
            if (sum < 0) printf("");
        }
        double avg = total / TRIALS;
        printf("16\t%.3f\t\t%.2fx\n", avg, baseline/avg);
    }
    
    // U=32
    {
        double total = 0.0;
        for (int t = 0; t < TRIALS; t++) {
            float sum = 0.0f;
            clock_t start = clock();
            int i;
            for (i = 0; i < N-31; i += 32) 
                sum += a[i] + a[i+1] + a[i+2] + a[i+3] +
                       a[i+4] + a[i+5] + a[i+6] + a[i+7] +
                       a[i+8] + a[i+9] + a[i+10] + a[i+11] +
                       a[i+12] + a[i+13] + a[i+14] + a[i+15] +
                       a[i+16] + a[i+17] + a[i+18] + a[i+19] +
                       a[i+20] + a[i+21] + a[i+22] + a[i+23] +
                       a[i+24] + a[i+25] + a[i+26] + a[i+27] +
                       a[i+28] + a[i+29] + a[i+30] + a[i+31];
            for (; i < N; i++) sum += a[i];
            clock_t end = clock();
            total += ((double)(end - start) / CLOCKS_PER_SEC) * 1000;
            if (sum < 0) printf("");
        }
        double avg = total / TRIALS;
        printf("32\t%.3f\t\t%.2fx\n", avg, baseline/avg);
    }
    
    free(a);
}

// ============================================================================
// INT BENCHMARKS
// ============================================================================
void benchmark_int() {
    int *a = malloc(N * sizeof(int));
    for (int i = 0; i < N; i++) a[i] = 1;
    
    printf("\n=== INT (4 bytes) ===\n");
    printf("Unroll\tTime(ms)\tSpeedup\n");
    printf("------------------------------------\n");
    
    double baseline = 0.0;
    
    // U=1
    {
        double total = 0.0;
        for (int t = 0; t < TRIALS; t++) {
            int sum = 0;
            clock_t start = clock();
            for (int i = 0; i < N; i++) sum += a[i];
            clock_t end = clock();
            total += ((double)(end - start) / CLOCKS_PER_SEC) * 1000;
            if (sum < 0) printf("");
        }
        baseline = total / TRIALS;
        printf("1\t%.3f\t\t1.00x\n", baseline);
    }
    
    // U=2
    {
        double total = 0.0;
        for (int t = 0; t < TRIALS; t++) {
            int sum = 0;
            clock_t start = clock();
            int i;
            for (i = 0; i < N-1; i += 2) sum += a[i] + a[i+1];
            for (; i < N; i++) sum += a[i];
            clock_t end = clock();
            total += ((double)(end - start) / CLOCKS_PER_SEC) * 1000;
            if (sum < 0) printf("");
        }
        double avg = total / TRIALS;
        printf("2\t%.3f\t\t%.2fx\n", avg, baseline/avg);
    }
    
    // U=4
    {
        double total = 0.0;
        for (int t = 0; t < TRIALS; t++) {
            int sum = 0;
            clock_t start = clock();
            int i;
            for (i = 0; i < N-3; i += 4) 
                sum += a[i] + a[i+1] + a[i+2] + a[i+3];
            for (; i < N; i++) sum += a[i];
            clock_t end = clock();
            total += ((double)(end - start) / CLOCKS_PER_SEC) * 1000;
            if (sum < 0) printf("");
        }
        double avg = total / TRIALS;
        printf("4\t%.3f\t\t%.2fx\n", avg, baseline/avg);
    }
    
    // U=8
    {
        double total = 0.0;
        for (int t = 0; t < TRIALS; t++) {
            int sum = 0;
            clock_t start = clock();
            int i;
            for (i = 0; i < N-7; i += 8) 
                sum += a[i] + a[i+1] + a[i+2] + a[i+3] +
                       a[i+4] + a[i+5] + a[i+6] + a[i+7];
            for (; i < N; i++) sum += a[i];
            clock_t end = clock();
            total += ((double)(end - start) / CLOCKS_PER_SEC) * 1000;
            if (sum < 0) printf("");
        }
        double avg = total / TRIALS;
        printf("8\t%.3f\t\t%.2fx\n", avg, baseline/avg);
    }
    
    // U=16
    {
        double total = 0.0;
        for (int t = 0; t < TRIALS; t++) {
            int sum = 0;
            clock_t start = clock();
            int i;
            for (i = 0; i < N-15; i += 16) 
                sum += a[i] + a[i+1] + a[i+2] + a[i+3] +
                       a[i+4] + a[i+5] + a[i+6] + a[i+7] +
                       a[i+8] + a[i+9] + a[i+10] + a[i+11] +
                       a[i+12] + a[i+13] + a[i+14] + a[i+15];
            for (; i < N; i++) sum += a[i];
            clock_t end = clock();
            total += ((double)(end - start) / CLOCKS_PER_SEC) * 1000;
            if (sum < 0) printf("");
        }
        double avg = total / TRIALS;
        printf("16\t%.3f\t\t%.2fx\n", avg, baseline/avg);
    }
    
    // U=32
    {
        double total = 0.0;
        for (int t = 0; t < TRIALS; t++) {
            int sum = 0;
            clock_t start = clock();
            int i;
            for (i = 0; i < N-31; i += 32) 
                sum += a[i] + a[i+1] + a[i+2] + a[i+3] +
                       a[i+4] + a[i+5] + a[i+6] + a[i+7] +
                       a[i+8] + a[i+9] + a[i+10] + a[i+11] +
                       a[i+12] + a[i+13] + a[i+14] + a[i+15] +
                       a[i+16] + a[i+17] + a[i+18] + a[i+19] +
                       a[i+20] + a[i+21] + a[i+22] + a[i+23] +
                       a[i+24] + a[i+25] + a[i+26] + a[i+27] +
                       a[i+28] + a[i+29] + a[i+30] + a[i+31];
            for (; i < N; i++) sum += a[i];
            clock_t end = clock();
            total += ((double)(end - start) / CLOCKS_PER_SEC) * 1000;
            if (sum < 0) printf("");
        }
        double avg = total / TRIALS;
        printf("32\t%.3f\t\t%.2fx\n", avg, baseline/avg);
    }
    
    free(a);
}

// ============================================================================
// SHORT BENCHMARKS
// ============================================================================
void benchmark_short() {
    short *a = malloc(N * sizeof(short));
    for (int i = 0; i < N; i++) a[i] = 1;
    
    printf("\n=== SHORT (2 bytes) ===\n");
    printf("Unroll\tTime(ms)\tSpeedup\n");
    printf("------------------------------------\n");
    
    double baseline = 0.0;
    
    // U=1
    {
        double total = 0.0;
        for (int t = 0; t < TRIALS; t++) {
            int sum = 0;
            clock_t start = clock();
            for (int i = 0; i < N; i++) sum += a[i];
            clock_t end = clock();
            total += ((double)(end - start) / CLOCKS_PER_SEC) * 1000;
            if (sum < 0) printf("");
        }
        baseline = total / TRIALS;
        printf("1\t%.3f\t\t1.00x\n", baseline);
    }
    
    // U=2
    {
        double total = 0.0;
        for (int t = 0; t < TRIALS; t++) {
            int sum = 0;
            clock_t start = clock();
            int i;
            for (i = 0; i < N-1; i += 2) sum += a[i] + a[i+1];
            for (; i < N; i++) sum += a[i];
            clock_t end = clock();
            total += ((double)(end - start) / CLOCKS_PER_SEC) * 1000;
            if (sum < 0) printf("");
        }
        double avg = total / TRIALS;
        printf("2\t%.3f\t\t%.2fx\n", avg, baseline/avg);
    }
    
    // U=4
    {
        double total = 0.0;
        for (int t = 0; t < TRIALS; t++) {
            int sum = 0;
            clock_t start = clock();
            int i;
            for (i = 0; i < N-3; i += 4) 
                sum += a[i] + a[i+1] + a[i+2] + a[i+3];
            for (; i < N; i++) sum += a[i];
            clock_t end = clock();
            total += ((double)(end - start) / CLOCKS_PER_SEC) * 1000;
            if (sum < 0) printf("");
        }
        double avg = total / TRIALS;
        printf("4\t%.3f\t\t%.2fx\n", avg, baseline/avg);
    }
    
    // U=8
    {
        double total = 0.0;
        for (int t = 0; t < TRIALS; t++) {
            int sum = 0;
            clock_t start = clock();
            int i;
            for (i = 0; i < N-7; i += 8) 
                sum += a[i] + a[i+1] + a[i+2] + a[i+3] +
                       a[i+4] + a[i+5] + a[i+6] + a[i+7];
            for (; i < N; i++) sum += a[i];
            clock_t end = clock();
            total += ((double)(end - start) / CLOCKS_PER_SEC) * 1000;
            if (sum < 0) printf("");
        }
        double avg = total / TRIALS;
        printf("8\t%.3f\t\t%.2fx\n", avg, baseline/avg);
    }
    
    // U=16
    {
        double total = 0.0;
        for (int t = 0; t < TRIALS; t++) {
            int sum = 0;
            clock_t start = clock();
            int i;
            for (i = 0; i < N-15; i += 16) 
                sum += a[i] + a[i+1] + a[i+2] + a[i+3] +
                       a[i+4] + a[i+5] + a[i+6] + a[i+7] +
                       a[i+8] + a[i+9] + a[i+10] + a[i+11] +
                       a[i+12] + a[i+13] + a[i+14] + a[i+15];
            for (; i < N; i++) sum += a[i];
            clock_t end = clock();
            total += ((double)(end - start) / CLOCKS_PER_SEC) * 1000;
            if (sum < 0) printf("");
        }
        double avg = total / TRIALS;
        printf("16\t%.3f\t\t%.2fx\n", avg, baseline/avg);
    }
    
    // U=32
    {
        double total = 0.0;
        for (int t = 0; t < TRIALS; t++) {
            int sum = 0;
            clock_t start = clock();
            int i;
            for (i = 0; i < N-31; i += 32) 
                sum += a[i] + a[i+1] + a[i+2] + a[i+3] +
                       a[i+4] + a[i+5] + a[i+6] + a[i+7] +
                       a[i+8] + a[i+9] + a[i+10] + a[i+11] +
                       a[i+12] + a[i+13] + a[i+14] + a[i+15] +
                       a[i+16] + a[i+17] + a[i+18] + a[i+19] +
                       a[i+20] + a[i+21] + a[i+22] + a[i+23] +
                       a[i+24] + a[i+25] + a[i+26] + a[i+27] +
                       a[i+28] + a[i+29] + a[i+30] + a[i+31];
            for (; i < N; i++) sum += a[i];
            clock_t end = clock();
            total += ((double)(end - start) / CLOCKS_PER_SEC) * 1000;
            if (sum < 0) printf("");
        }
        double avg = total / TRIALS;
        printf("32\t%.3f\t\t%.2fx\n", avg, baseline/avg);
    }
    
    free(a);
}

// ============================================================================
// MAIN
// ============================================================================
int main() {
    printf("======================================================\n");
    printf("  LOOP UNROLLING BENCHMARK - MULTIPLE TYPES\n");
    printf("======================================================\n");
    printf("Array size: N = %d elements\n", N);
    printf("Trials: %d\n", TRIALS);
    printf("======================================================\n");
    
    benchmark_double();
    benchmark_float();
    benchmark_int();
    benchmark_short();
    
    printf("\n======================================================\n");
    printf("MEMORY BANDWIDTH ANALYSIS\n");
    printf("======================================================\n");
    printf("Assuming BW = 20 GB/s (adjust for your system)\n");
    printf("T_min = (N × sizeof(type)) / BW\n\n");
    
    double BW_GBps = 20.0;
    double N_MB_double = (N * 8.0) / (1024*1024);
    double N_MB_float = (N * 4.0) / (1024*1024);
    double N_MB_int = (N * 4.0) / (1024*1024);
    double N_MB_short = (N * 2.0) / (1024*1024);
    
    printf("Type\t\tSize\tData(MB)\tT_min(ms)\n");
    printf("--------------------------------------------------\n");
    printf("double\t\t8 B\t%.2f\t\t%.3f\n", N_MB_double, (N_MB_double/1000)/BW_GBps*1000);
    printf("float\t\t4 B\t%.2f\t\t%.3f\n", N_MB_float, (N_MB_float/1000)/BW_GBps*1000);
    printf("int\t\t4 B\t%.2f\t\t%.3f\n", N_MB_int, (N_MB_int/1000)/BW_GBps*1000);
    printf("short\t\t2 B\t%.2f\t\t%.3f\n", N_MB_short, (N_MB_short/1000)/BW_GBps*1000);
  
    return 0;
}
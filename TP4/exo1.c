#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#ifdef _OPENMP
#include <omp.h>
#endif

#define N 1000000

int main() {
    double *A = malloc(N * sizeof(double));
    if (!A) return 1;

    srand(0);
    for (int i = 0; i < N; i++)
        A[i] = (double)rand() / RAND_MAX;

    double sum = 0.0;
    double max = 0.0;
    double stddev = 0.0;

    #pragma omp parallel for reduction(+:sum)
    for (int i = 0; i < N; i++)
        sum += A[i];

    double mean = sum / N;

    #pragma omp parallel for reduction(max:max)
    for (int i = 0; i < N; i++)
        if (A[i] > max)
            max = A[i];

    #pragma omp parallel for reduction(+:stddev)
    for (int i = 0; i < N; i++)
        stddev += (A[i] - mean) * (A[i] - mean);

    stddev = sqrt(stddev / N);

    printf("Sum = %f\n", sum);
    printf("Max = %f\n", max);
    printf("StdDev = %f\n", stddev);

    free(A);
    return 0;
}

#include <stdio.h>
#include <stdlib.h>
#ifdef _OPENMP
#include <omp.h>
#endif


#define N 1000

int main() {
    double *A = malloc(N * N * sizeof(double));
    if (!A) return 1;

    double sum = 0.0;

    double start = omp_get_wtime();

    #pragma omp parallel
    {
        #pragma omp master
        {
            for (int i = 0; i < N; i++)
                for (int j = 0; j < N; j++)
                    A[i*N + j] = (double)(i + j);
        }

        #pragma omp barrier

        #pragma omp for reduction(+:sum)
        for (int i = 0; i < N*N; i++)
            sum += A[i];

        #pragma omp single
        {
            printf("Sum = %f\n", sum);
        }
    }

    double end = omp_get_wtime();
    printf("Execution time = %f seconds\n", end - start);

    free(A);
    return 0;
}

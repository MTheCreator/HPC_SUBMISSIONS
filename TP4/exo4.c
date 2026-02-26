#include <stdio.h>
#include <stdlib.h>
#ifdef _OPENMP
#include <omp.h>
#endif

#define N 40000
#define M 600

void version1(double *lhs, double *rhs, double *mat) {
    #pragma omp parallel
    {
        #pragma omp for
        for (int c = 0; c < N; c++) {
            int offset = M * c;
            for (int r = 0; r < M; r++)
                lhs[r] += mat[r + offset] * rhs[c];
        }
    }
}

void version2(double *lhs, double *rhs, double *mat) {
    #pragma omp parallel
    {
        for (int c = 0; c < N; c++) {
            int offset = M * c;
            #pragma omp for schedule(dynamic) nowait
            for (int r = 0; r < M; r++)
                lhs[r] += mat[r + offset] * rhs[c];
        }
    }
}

void version3(double *lhs, double *rhs, double *mat) {
    #pragma omp parallel
    {
        for (int c = 0; c < N; c++) {
            int offset = M * c;
            #pragma omp for schedule(static) nowait
            for (int r = 0; r < M; r++)
                lhs[r] += mat[r + offset] * rhs[c];
        }
    }
}

int main(int argc, char **argv) {
    int version = atoi(argv[1]);

    double *mat = malloc(N * M * sizeof(double));
    double *rhs = malloc(N * sizeof(double));
    double *lhs = malloc(M * sizeof(double));

    for (int c = 0; c < N; c++) {
        rhs[c] = 1.0;
        for (int r = 0; r < M; r++)
            mat[r + c*M] = 1.0;
    }

    for (int r = 0; r < M; r++)
        lhs[r] = 0.0;

    double start = omp_get_wtime();

    if (version == 1) version1(lhs, rhs, mat);
    if (version == 2) version2(lhs, rhs, mat);
    if (version == 3) version3(lhs, rhs, mat);

    double end = omp_get_wtime();
    double time = end - start;

    double flops = 2.0 * N * M;
    double mflops = flops / (time * 1e6);

    printf("Time = %f\n", time);
    printf("MFLOPS = %f\n", mflops);

    free(mat);
    free(rhs);
    free(lhs);
    return 0;
}

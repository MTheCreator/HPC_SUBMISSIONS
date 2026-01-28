#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define IDX(i,j,n) ((i)*(n)+(j))

double wall_time() {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec + t.tv_nsec * 1e-9;
}

int main(int argc, char **argv) {
    if (argc != 3) {
        printf("Usage: %s N NB\n", argv[0]);
        return 1;
    }

    int N  = atoi(argv[1]);
    int NB = atoi(argv[2]);

    double *A = aligned_alloc(64, N*N*sizeof(double));
    double *B = aligned_alloc(64, N*N*sizeof(double));
    double *C = aligned_alloc(64, N*N*sizeof(double));

    for (int i = 0; i < N*N; i++) {
        A[i] = 1.0;
        B[i] = 1.0;
        C[i] = 0.0;
    }

    double t0 = wall_time();

    for (int ii = 0; ii < N; ii += NB)
        for (int jj = 0; jj < N; jj += NB)
            for (int kk = 0; kk < N; kk += NB)
                for (int i = ii; i < ii + NB && i < N; i++)
                    for (int k = kk; k < kk + NB && k < N; k++)
                        for (int j = jj; j < jj + NB && j < N; j++)
                            C[IDX(i,j,N)] += A[IDX(i,k,N)] * B[IDX(k,j,N)];

    double t1 = wall_time();

    double time = t1 - t0;
    double flops = 2.0 * N * N * N;
    double gflops = flops / time / 1e9;

    printf("N=%d NB=%d Time=%.3f s GFLOP/s=%.2f\n", N, NB, time, gflops);

    free(A); free(B); free(C);
    return 0;
}

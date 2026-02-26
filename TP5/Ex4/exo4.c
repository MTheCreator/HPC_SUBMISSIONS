#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

void matrixVectorMult(double* A, double* b, double* x, int size) {
    for (int i = 0; i < size; ++i) {
        x[i] = 0.0;
        for (int j = 0; j < size; ++j) {
            x[i] += A[i * size + j] * b[j];
        }
    }
}

int main(int argc, char* argv[]) {
    int rank, size;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (argc != 2) {
        if (rank == 0) {
            printf("Usage: %s <matrix_size>\n", argv[0]);
        }
        MPI_Finalize();
        return 1;
    }

    int N = atoi(argv[1]);
    if (N <= 0) {
        if (rank == 0) {
            printf("Matrix size must be positive.\n");
        }
        MPI_Finalize();
        return 1;
    }

    double *A = NULL;
    double *b = NULL;
    double *x_serial = NULL;
    double *x_parallel = NULL;

    // Calculate rows per process (handle uneven distribution)
    int base_rows = N / size;
    int extra_rows = N % size;
    int local_rows = base_rows + (rank < extra_rows ? 1 : 0);

    // Arrays to store distribution information
    int *sendcounts = NULL;
    int *displs = NULL;

    // Declare timing variables at outer scope so they are accessible everywhere
    double start_serial = 0.0, end_serial = 0.0;
    double start_parallel, end_parallel;

    if (rank == 0) {
        // Allocate and initialize on root
        A = malloc(N * N * sizeof(double));
        b = malloc(N * sizeof(double));
        x_serial = malloc(N * sizeof(double));
        x_parallel = malloc(N * sizeof(double));

        if (!A || !b || !x_serial || !x_parallel) {
            printf("Memory allocation failed.\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        srand48(42);

        // Fill A[0][:100] with random values
        int limit = (N < 100) ? N : 100;
        for (int j = 0; j < limit; ++j)
            A[0 * N + j] = drand48();

        // Copy A[0][:100] into A[1][100:200] if possible
        if (N > 1 && N > 100) {
            int copy_len = (N - 100 < 100) ? (N - 100) : 100;
            for (int j = 0; j < copy_len; ++j)
                A[1 * N + (100 + j)] = A[0 * N + j];
        }

        // Set diagonal
        for (int i = 0; i < N; ++i)
            A[i * N + i] = drand48();

        // Fill vector b
        for (int i = 0; i < N; ++i)
            b[i] = drand48();

        // Compute serial result for verification
        start_serial = MPI_Wtime();
        matrixVectorMult(A, b, x_serial, N);
        end_serial = MPI_Wtime();
        printf("Serial time: %f seconds\n", end_serial - start_serial);

        // Prepare sendcounts and displacements for Scatterv
        sendcounts = malloc(size * sizeof(int));
        displs = malloc(size * sizeof(int));

        int offset = 0;
        for (int i = 0; i < size; i++) {
            int rows = base_rows + (i < extra_rows ? 1 : 0);
            sendcounts[i] = rows * N;
            displs[i] = offset;
            offset += rows * N;
        }
    }

    // Allocate local buffers
    double *local_A = malloc(local_rows * N * sizeof(double));
    double *local_x = malloc(local_rows * sizeof(double));

    if (!local_A || !local_x) {
        printf("Process %d: Memory allocation failed.\n", rank);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    // All processes need b vector
    if (rank != 0) {
        b = malloc(N * sizeof(double));
    }

    start_parallel = MPI_Wtime();

    // Broadcast b to all processes
    MPI_Bcast(b, N, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // Scatter rows of A to all processes
    MPI_Scatterv(A, sendcounts, displs, MPI_DOUBLE,
                 local_A, local_rows * N, MPI_DOUBLE,
                 0, MPI_COMM_WORLD);

    // Compute local matrix-vector product
    for (int i = 0; i < local_rows; i++) {
        local_x[i] = 0.0;
        for (int j = 0; j < N; j++) {
            local_x[i] += local_A[i * N + j] * b[j];
        }
    }

    // Prepare for gathering results
    int *recvcounts = NULL;
    int *recv_displs = NULL;

    if (rank == 0) {
        recvcounts = malloc(size * sizeof(int));
        recv_displs = malloc(size * sizeof(int));

        int offset = 0;
        for (int i = 0; i < size; i++) {
            int rows = base_rows + (i < extra_rows ? 1 : 0);
            recvcounts[i] = rows;
            recv_displs[i] = offset;
            offset += rows;
        }
    }

    // Gather results
    MPI_Gatherv(local_x, local_rows, MPI_DOUBLE,
                x_parallel, recvcounts, recv_displs, MPI_DOUBLE,
                0, MPI_COMM_WORLD);

    end_parallel = MPI_Wtime();

    if (rank == 0) {
        printf("Parallel time: %f seconds\n", end_parallel - start_parallel);

        // Verify correctness
        double max_error = 0.0;
        for (int i = 0; i < N; i++) {
            double diff = fabs(x_parallel[i] - x_serial[i]);
            if (diff > max_error)
                max_error = diff;
        }
        printf("Maximum difference between parallel and serial result: %e\n", max_error);

        // Calculate speedup and efficiency
        double speedup = (end_serial - start_serial) / (end_parallel - start_parallel);
        double efficiency = speedup / size;
        printf("Speedup: %.2f\n", speedup);
        printf("Efficiency: %.2f%%\n", efficiency * 100);

        free(A);
        free(x_serial);
        free(x_parallel);
        free(sendcounts);
        free(displs);
        free(recvcounts);
        free(recv_displs);
    }

    free(b);
    free(local_A);
    free(local_x);

    MPI_Finalize();
    return 0;
}
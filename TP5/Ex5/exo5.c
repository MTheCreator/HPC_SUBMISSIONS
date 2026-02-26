#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

double calculate_pi_serial(long N) {
    double sum = 0.0;
    double step = 1.0 / N;
    
    for (long i = 0; i < N; i++) {
        double x = (i + 0.5) * step;
        sum += 1.0 / (1.0 + x * x);
    }
    
    return 4.0 * sum * step;
}

int main(int argc, char* argv[]) {
    int rank, size;
    
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    if (argc != 2) {
        if (rank == 0) {
            printf("Usage: %s <N>\n", argv[0]);
        }
        MPI_Finalize();
        return 1;
    }
    
    long N = atol(argv[1]);
    if (N <= 0) {
        if (rank == 0) {
            printf("N must be positive.\n");
        }
        MPI_Finalize();
        return 1;
    }
    
    double pi_serial = 0.0;
    double serial_time = 0.0;
    
    // Serial calculation (only on rank 0)
    if (rank == 0) {
        double start = MPI_Wtime();
        pi_serial = calculate_pi_serial(N);
        double end = MPI_Wtime();
        serial_time = end - start;
        printf("Serial calculation:\n");
        printf("  Pi ≈ %.15f\n", pi_serial);
        printf("  Error = %.15e\n", fabs(pi_serial - M_PI));
        printf("  Time = %f seconds\n\n", serial_time);
    }
    
    // Parallel calculation
    double start_parallel = MPI_Wtime();
    
    // Calculate work distribution (handle uneven division)
    long base_count = N / size;
    long remainder = N % size;
    
    // Each process gets base_count items, first 'remainder' processes get one extra
    long local_start, local_count;
    
    if (rank < remainder) {
        local_count = base_count + 1;
        local_start = rank * local_count;
    } else {
        local_count = base_count;
        local_start = rank * base_count + remainder;
    }
    
    // Calculate local sum
    double local_sum = 0.0;
    double step = 1.0 / N;
    
    for (long i = local_start; i < local_start + local_count; i++) {
        double x = (i + 0.5) * step;
        local_sum += 1.0 / (1.0 + x * x);
    }
    
    // Reduce all local sums to get total sum
    double total_sum = 0.0;
    MPI_Reduce(&local_sum, &total_sum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    
    double end_parallel = MPI_Wtime();
    
    if (rank == 0) {
        double pi_parallel = 4.0 * total_sum * step;
        double parallel_time = end_parallel - start_parallel;
        
        printf("Parallel calculation (with %d processes):\n", size);
        printf("  Pi ≈ %.15f\n", pi_parallel);
        printf("  Error = %.15e\n", fabs(pi_parallel - M_PI));
        printf("  Time = %f seconds\n", parallel_time);
        
        // Calculate speedup and efficiency
        double speedup = serial_time / parallel_time;
        double efficiency = speedup / size;
        
        printf("\nPerformance metrics:\n");
        printf("  Speedup: %.2f\n", speedup);
        printf("  Efficiency: %.2f%%\n", efficiency * 100);
        
        // Verify parallel and serial results match
        double diff = fabs(pi_parallel - pi_serial);
        printf("  Difference between serial and parallel: %e\n", diff);
        
        if (diff < 1e-10) {
            printf("  ✓ Results match!\n");
        } else {
            printf("  ✗ Results do not match!\n");
        }
    }
    
    MPI_Finalize();
    return 0;
}
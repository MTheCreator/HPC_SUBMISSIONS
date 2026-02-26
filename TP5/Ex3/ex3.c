#include <stdio.h>
#include <mpi.h>

int main(int argc, char* argv[]) {
    int rank, size;
    int value;
    
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    if (size < 2) {
        if (rank == 0) {
            printf("This program requires at least 2 processes.\n");
        }
        MPI_Finalize();
        return 1;
    }
    
    if (rank == 0) {
        // Process 0 reads value from user
        printf("Process 0: Enter an integer value: ");
        fflush(stdout);
        scanf("%d", &value);
        
        // Add rank and print
        value += rank;
        printf("Process %d: Current value = %d\n", rank, value);
        
        // Send to process 1
        MPI_Send(&value, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
    } else {
        // Receive from previous process
        int prev_rank = rank - 1;
        MPI_Recv(&value, 1, MPI_INT, prev_rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        
        // Add own rank
        value += rank;
        printf("Process %d: Current value = %d\n", rank, value);
        
        // Send to next process (if not the last)
        if (rank < size - 1) {
            int next_rank = rank + 1;
            MPI_Send(&value, 1, MPI_INT, next_rank, 0, MPI_COMM_WORLD);
        } else {
            printf("\nFinal value after passing through all processes: %d\n", value);
        }
    }
    
    MPI_Finalize();
    return 0;
}
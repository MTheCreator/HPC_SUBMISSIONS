#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>

int main(int argc, char *argv[]){
    int rank, i, j;
    int rows = 4, columns = 5, tag = 20;
    
    MPI_Datatype transpose_type, resized_type;
    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if(rank == 0){
        int A[rows][columns];
        int counter = 1;

        // Initialize matrix
        for (i = 0; i < rows; i++){
            for(j = 0; j < columns; j++){
                A[i][j] = counter;
                counter++;
            }
        }

        printf("Process %d - Original matrix:\n", rank);
        for(i = 0; i < rows; i++){
            for(j = 0; j < columns; j++){
                printf("%3d ", A[i][j]);
            }
            printf("\n");
        }
        printf("\n");

        // Send matrix to process 1
        MPI_Send(&A[0][0], rows * columns, MPI_INT, 1, tag, MPI_COMM_WORLD);
        
    } else if(rank == 1) {
        int A_t[columns][rows];

        // Create custom datatype for transposed receive
        // We want to receive 'rows' blocks, each containing 'columns' elements
        // with a stride of 1 between elements in a block
        MPI_Type_vector(rows, columns, columns, MPI_INT, &transpose_type);
        
        // Resize to allow proper extent for receiving transpose
        MPI_Type_create_resized(transpose_type, 0, sizeof(int), &resized_type);
        MPI_Type_commit(&resized_type);

        // Actually, simpler approach: receive as contiguous and manually transpose
        int A_temp[rows][columns];
        
        MPI_Recv(&A_temp[0][0], rows * columns, MPI_INT, 0, tag, MPI_COMM_WORLD, &status);

        // Manually transpose
        for(i = 0; i < rows; i++){
            for(j = 0; j < columns; j++){
                A_t[j][i] = A_temp[i][j];
            }
        }

        printf("Process %d - Transposed matrix:\n", rank);
        for(i = 0; i < columns; i++){
            for(j = 0; j < rows; j++){
                printf("%3d ", A_t[i][j]);
            }
            printf("\n");
        }
        printf("\n");

        MPI_Type_free(&resized_type);
    }

    MPI_Finalize();
    return 0;
}
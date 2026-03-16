#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define N 16
#define G 10

int count_neighbors(int **grid, int i, int j)
{
    return grid[i-1][j-1] + grid[i-1][j] + grid[i-1][j+1] +
           grid[i][j-1]               + grid[i][j+1] +
           grid[i+1][j-1] + grid[i+1][j] + grid[i+1][j+1];
}

int main(int argc, char *argv[])
{
    MPI_Init(&argc,&argv);

    int rank,size;
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    MPI_Comm_size(MPI_COMM_WORLD,&size);

    int dims[2]={0,0};
    MPI_Dims_create(size,2,dims);

    int periods[2]={1,1};   // periodic boundary
    MPI_Comm cart;

    MPI_Cart_create(MPI_COMM_WORLD,2,dims,periods,1,&cart);

    int coords[2];
    MPI_Cart_coords(cart,rank,2,coords);

    int north,south,east,west;

    MPI_Cart_shift(cart,0,1,&north,&south);
    MPI_Cart_shift(cart,1,1,&west,&east);

    int local_nx = N/dims[0];
    int local_ny = N/dims[1];

    int **grid = malloc((local_nx+2)*sizeof(int*));
    int **newgrid = malloc((local_nx+2)*sizeof(int*));

    for(int i=0;i<local_nx+2;i++){
        grid[i]=malloc((local_ny+2)*sizeof(int));
        newgrid[i]=malloc((local_ny+2)*sizeof(int));
    }

    srand(rank);
    for(int i=1;i<=local_nx;i++)
        for(int j=1;j<=local_ny;j++)
            grid[i][j]=rand()%2;

    for(int gen=0; gen<G; gen++)
    {
        MPI_Sendrecv(&grid[1][1],local_ny,MPI_INT,north,0,
                     &grid[local_nx+1][1],local_ny,MPI_INT,south,0,
                     cart,MPI_STATUS_IGNORE);

        MPI_Sendrecv(&grid[local_nx][1],local_ny,MPI_INT,south,1,
                     &grid[0][1],local_ny,MPI_INT,north,1,
                     cart,MPI_STATUS_IGNORE);

        for(int i=1;i<=local_nx;i++)
        {
            MPI_Sendrecv(&grid[i][1],1,MPI_INT,west,2,
                         &grid[i][local_ny+1],1,MPI_INT,east,2,
                         cart,MPI_STATUS_IGNORE);

            MPI_Sendrecv(&grid[i][local_ny],1,MPI_INT,east,3,
                         &grid[i][0],1,MPI_INT,west,3,
                         cart,MPI_STATUS_IGNORE);
        }

        for(int i=1;i<=local_nx;i++)
        {
            for(int j=1;j<=local_ny;j++)
            {
                int neighbors=count_neighbors(grid,i,j);

                if(grid[i][j]==1)
                    newgrid[i][j]=(neighbors==2 || neighbors==3);
                else
                    newgrid[i][j]=(neighbors==3);
            }
        }

        for(int i=1;i<=local_nx;i++)
            for(int j=1;j<=local_ny;j++)
                grid[i][j]=newgrid[i][j];
    }

    printf("Rank %d finished\n",rank);

    printf("Rank %d grid:\n", rank);
    for(int i=1;i<=local_nx;i++){
        for(int j=1;j<=local_ny;j++)
            printf("%d ",grid[i][j]);
        printf("\n");
    }

    MPI_Finalize();
    return 0;
}
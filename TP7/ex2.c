#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define NX 64
#define NY 64
#define MAX_ITER 10000
#define TOL 1e-6

double f(double x,double y)
{
    return 2*(x*x - x + y*y - y);
}

int main(int argc,char **argv)
{
    MPI_Init(&argc,&argv);

    int rank,size;
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    MPI_Comm_size(MPI_COMM_WORLD,&size);

    int dims[2]={0,0};
    MPI_Dims_create(size,2,dims);

    int periods[2]={0,0};
    MPI_Comm cart;

    MPI_Cart_create(MPI_COMM_WORLD,2,dims,periods,1,&cart);

    int north,south,east,west;

    MPI_Cart_shift(cart,0,1,&north,&south);
    MPI_Cart_shift(cart,1,1,&west,&east);

    int local_nx = NX/dims[0];
    int local_ny = NY/dims[1];

    double hx = 1.0/(NX-1);
    double hy = 1.0/(NY-1);

    double coef1 = 0.5*hx*hx*hy*hy/(hx*hx+hy*hy);
    double coef2 = 1.0/(hx*hx);
    double coef3 = 1.0/(hy*hy);

    double **u = malloc((local_nx+2)*sizeof(double*));
    double **unew = malloc((local_nx+2)*sizeof(double*));

    for(int i=0;i<local_nx+2;i++){
        u[i]=calloc(local_ny+2,sizeof(double));
        unew[i]=calloc(local_ny+2,sizeof(double));
    }

    int iter=0;
    double global_error=1;

    while(global_error>TOL && iter<MAX_ITER)
    {
        for(int i=1;i<=local_nx;i++)
        {
            MPI_Sendrecv(&u[i][1],1,MPI_DOUBLE,west,0,
                         &u[i][local_ny+1],1,MPI_DOUBLE,east,0,
                         cart,MPI_STATUS_IGNORE);

            MPI_Sendrecv(&u[i][local_ny],1,MPI_DOUBLE,east,1,
                         &u[i][0],1,MPI_DOUBLE,west,1,
                         cart,MPI_STATUS_IGNORE);
        }

        MPI_Sendrecv(&u[1][1],local_ny,MPI_DOUBLE,north,2,
                     &u[local_nx+1][1],local_ny,MPI_DOUBLE,south,2,
                     cart,MPI_STATUS_IGNORE);

        MPI_Sendrecv(&u[local_nx][1],local_ny,MPI_DOUBLE,south,3,
                     &u[0][1],local_ny,MPI_DOUBLE,north,3,
                     cart,MPI_STATUS_IGNORE);

        double local_error=0;

        for(int i=1;i<=local_nx;i++)
        {
            for(int j=1;j<=local_ny;j++)
            {
                double x = (i + rank*local_nx)*hx;
                double y = (j + rank*local_ny)*hy;

                unew[i][j] = coef1*(coef2*(u[i+1][j]+u[i-1][j]) +
                                    coef3*(u[i][j+1]+u[i][j-1]) -
                                    f(x,y));

                double diff=fabs(unew[i][j]-u[i][j]);
                if(diff>local_error) local_error=diff;
            }
        }

        MPI_Allreduce(&local_error,&global_error,1,MPI_DOUBLE,MPI_MAX,cart);

        for(int i=1;i<=local_nx;i++)
            for(int j=1;j<=local_ny;j++)
                u[i][j]=unew[i][j];

        iter++;

        if(rank==0 && iter%100==0)
            printf("Iteration %d error %e\n",iter,global_error);
    }

    if(rank==0)
        printf("Converged after %d iterations\n",iter);

    MPI_Finalize();
    return 0;
}
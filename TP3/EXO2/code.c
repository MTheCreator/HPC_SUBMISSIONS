#include <stdio.h>
#ifdef _OPENMP
#include <omp.h>
#endif

static long num_steps = 100000;
double step;

int main(void){
    int i; 
    double x, pi, sum = 0.0;
    double start_time, end_time;
    step = 1.0 / (double) num_steps;

    start_time = omp_get_wtime();
    # pragma omp parallel private(i, x) reduction(+:sum)
    {
        int id = omp_get_thread_num();
        int num_threads = omp_get_num_threads();

        for (i = id; i < num_steps; i += num_threads) {
            x = (i + 0.5) * step;
            sum = sum + 4.0 / (1.0 + x * x);
        }
    } 

    pi = step * sum;   

    end_time = omp_get_wtime();

    printf("PI = %1.5f\n", pi);
    printf("Time = %f seconds\n", end_time - start_time);

    return 0;
}

#include <stdio.h>
#include <stdlib.h>

#ifdef _OPENMP
#include <omp.h>
#endif

int main(void) {
    int p;
    printf("Enter # of threads: ");
    scanf("%d", &p);
    //int p = 5;

    #pragma omp parallel num_threads(p)
    {
        int thread_id = omp_get_thread_num();
        printf("Hello World! from thread %d\n", thread_id);
    }

    return 0;
}
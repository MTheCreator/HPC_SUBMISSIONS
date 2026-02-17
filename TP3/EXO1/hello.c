#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#ifdef _OPENMP
#include <omp.h>
#endif

int main(void){
    int p ;
    # pragma omp parallel private(p)
    {
        p = omp_get_thread_num();
        printf("Hello World! from thread %d\n", p);
    }
    return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 100000000

int main() {
    double a = 1.1, b = 1.2;
    double x = 0.0, y = 0.0;
    clock_t start, end;

    start = clock();
    for (int i = 0; i < N; i++) {
        x = a * b + x;
        y = a * b + y;
    }
    end = clock();

    printf("x = %f, y = %f, time = %f sec\n", x, y, (double)(end - start) / CLOCKS_PER_SEC);

    return 0;
}
#include <stdio.h>
#include <time.h>

int main() {
    clock_t start = clock();

    for (long long i = 0; i < 1000000000; i++);

    clock_t end = clock();
    double time_taken = (double)(end - start) / CLOCKS_PER_SEC;

    printf("Time taken in C: %f seconds\n", time_taken);
    return 0;
}


#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

int main() {
    int N = 100000000;
    int dentro_do_circulo = 0;
    double x, y;
    double start, end;

    start = omp_get_wtime();

    #pragma omp parallel shared(N) private(x, y)
    {
        #pragma omp for lastprivate(dentro_do_circulo)
        for (int i = 0; i < N; i++) {
            x = (double)rand() / RAND_MAX;
            y = (double)rand() / RAND_MAX;

            if (x * x + y * y <= 1.0)
                dentro_do_circulo++;
        }
    }

    double pi = 4.0 * dentro_do_circulo / N;
    end = omp_get_wtime();

    printf("Estimativa de Pi: %f\n", pi);
    printf("Tempo: %f\n", end - start);

    return 0;
}

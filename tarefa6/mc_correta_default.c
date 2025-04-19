#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

int main() {
    long N = 100000000;
    long dentro_do_circulo = 0;
    double x, y;
    double start, end;

    start = omp_get_wtime();
    #pragma omp parallel default(none) shared(N, dentro_do_circulo) private(x, y)
    {
        unsigned int seed = time(NULL) ^ omp_get_thread_num();
        #pragma omp for reduction(+:dentro_do_circulo)
        for (long i = 0; i < N; i++) {
            x = (double)rand_r(&seed) / RAND_MAX;
            y = (double)rand_r(&seed) / RAND_MAX;
            if (x * x + y * y <= 1.0)
                dentro_do_circulo++;
        }
    }
    end = omp_get_wtime();

    double pi = 4.0 * (double)dentro_do_circulo / (double)N;
    printf("Estimativa de pi: %f\n", pi);
    printf("Tempo: %f s\n", end - start);

    return 0;
}

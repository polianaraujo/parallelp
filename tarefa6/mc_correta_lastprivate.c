#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

int main() {
    const int N = 100000000;
    int dentro_do_circulo = 0;
    double x, y;
    double start, end;

    start = omp_get_wtime();

    #pragma omp parallel private(x, y)
    {
        int local_dentro = 0;  // Variável local para cada thread
        unsigned int seed = omp_get_thread_num();  // Seed única para rand_r

        #pragma omp for nowait
        for (int i = 0; i < N; i++) {
            x = (double)rand_r(&seed) / RAND_MAX;
            y = (double)rand_r(&seed) / RAND_MAX;

            if (x * x + y * y <= 1.0)
                local_dentro++;
        }

        // Combinação segura dos resultados
        #pragma omp atomic
        dentro_do_circulo += local_dentro;
    }

    double pi = 4.0 * dentro_do_circulo / N;
    end = omp_get_wtime();

    printf("Estimativa de Pi: %f\n", pi);
    printf("Tempo: %f\n", end - start);

    return 0;
}
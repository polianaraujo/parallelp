#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

int main() {
    long int N = 1000000;
    long int dentro_do_circulo = 0;

    #pragma omp parallel
    {
        unsigned int seed = time(NULL) ^ omp_get_thread_num();  // semente única por thread

        #pragma omp for reduction(+:dentro_do_circulo)
        for (long int i = 0; i < N; i++) {
            double x = (double)rand_r(&seed) / RAND_MAX;
            double y = (double)rand_r(&seed) / RAND_MAX;

            if (x*x + y*y <= 1.0) {
                dentro_do_circulo++;
            }
        }
    }

    double pi = 4.0 * dentro_do_circulo / N;
    printf("Estimativa de pi (paralelo correto) = %f\n", pi);

    return 0;
}

// Corrigindo com rand_r() (thread-safe) e controlando variáveis


#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

int main() {
    long int N = 1000000;
    long int dentro_do_circulo = 0;
    double x, y;

    srand(time(NULL));

    #pragma omp parallel for
    for (long int i = 0; i < N; i++) {
        x = (double)rand() / RAND_MAX;
        y = (double)rand() / RAND_MAX;

        if (x*x + y*y <= 1.0) {
            dentro_do_circulo++;  // condição de corrida!
        }
    }

    double pi = 4.0 * dentro_do_circulo / N;
    printf("Estimativa de pi (paralelo incorreto) = %f\n", pi);

    return 0;
}

// dentro_do_circulo++ é acessado por múltiplas threads simultaneamente → condição de corrida.

// rand() não é thread-safe → gera valores repetidos ou incorretos.
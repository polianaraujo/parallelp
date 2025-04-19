// o que o default faz?

// Implemente em C a estimativa estocástica de pi. Paralelize com "#pragma omp parallel for" e explique o resultado incorreto. Corrija reestruturando com "#pragma omp parallel" seguido de "#pragma omp for" e aplicando as cláusulas "private", "firstprivate", "lastprivate" e "shared". Teste diferentes combinações e explique como cada cláusula afeta o comportamento do programa. Comente também como a cláusula "default(none)" pode ajudar a tornar o escopo mais claro em programas complexos.


// # PRIMEIRO - mostrar como fazr uma estimativa estocástica em pi, com a técnica de Monte Carlo

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

int main() {
    long int N = 1000000;
    long int dentro_do_circulo = 0;
    double x, y;

    srand(time(NULL));

    for (long int i = 0; i < N; i++) {
        x = (double)rand() / RAND_MAX;
        y = (double)rand() / RAND_MAX;

        if (x*x + y*y <= 1.0) {
            dentro_do_circulo++;
        }
    }

    double pi = 4.0 * dentro_do_circulo / N;
    printf("Estimativa de pi = %f\n", pi);

    return 0;
}

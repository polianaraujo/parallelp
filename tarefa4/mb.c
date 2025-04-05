/*
Implemente dois programas paralelos em C com OpenMP: um limitado por memória, com somas simples em vetores, e outro limitado por CPU, com cálculos matemáticos intensivos. Paralelize com #pragma omp parallel for e meça o tempo de execução variando o número de threads. Analise quando o desempenho melhora, estabiliza ou piora, e reflita sobre como o multithreading de hardware pode ajudar em programas memory-bound, mas atrapalhar em programas compute-bound pela competição por recursos.
*/

#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define N 100000000  // 100 milhões

int main() {
    double *a = malloc(N * sizeof(double));
    double *b = malloc(N * sizeof(double));
    double *c = malloc(N * sizeof(double));
    
    for (long i = 0; i < N; i++) {
        a[i] = 1.0;
        b[i] = 2.0;
    }

    double start = omp_get_wtime();
    #pragma omp parallel for
    for (long i = 0; i < N; i++) {
        c[i] = a[i] + b[i];
    }
    double end = omp_get_wtime();

    printf("Tempo (memory-bound): %.4f segundos\n", end - start);

    free(a); free(b); free(c);
    return 0;
}

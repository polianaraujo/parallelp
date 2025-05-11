/*
Implemente dois programas paralelos em C com OpenMP: um limitado por memória, com somas simples em vetores, e outro limitado por CPU, com cálculos matemáticos intensivos. Paralelize com #pragma omp parallel for e meça o tempo de execução variando o número de threads. Analise quando o desempenho melhora, estabiliza ou piora, e reflita sobre como o multithreading de hardware pode ajudar em programas memory-bound, mas atrapalhar em programas compute-bound pela competição por recursos.
*/

#include <stdio.h>
#include <math.h>
#include <omp.h>

#define N 100000000

int main() {
    double result = 0.0;

    double start = omp_get_wtime();
    #pragma omp parallel for reduction(+:result)
    for (long i = 1; i <= N; i++) {
        result += sin(i) * log(i) / sqrt(i);
    }
    double end = omp_get_wtime();

    printf("Resultado: %.5f\n", result);
    printf("Tempo (compute-bound): %.4f segundos\n", end - start);
    
    return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <omp.h>
#include <unistd.h> // Para a função access()

#define VECTOR_SIZE 100000000
#define CSV_FILENAME "memory_results.csv"

int main() {
    double *a, *b, *c;
    struct timeval start_time, end_time;

    // Alocação de memória
    a = (double*)malloc(sizeof(double) * VECTOR_SIZE);
    b = (double*)malloc(sizeof(double) * VECTOR_SIZE);
    c = (double*)malloc(sizeof(double) * VECTOR_SIZE);
    if (a == NULL || b == NULL || c == NULL) {
        fprintf(stderr, "Falha na alocação de memória.\n");
        return 1;
    }

    // Inicialização (fora da medição de tempo)
    for (long i = 0; i < VECTOR_SIZE; i++) {
        a[i] = (double)i;
        b[i] = (double)i * 2.0;
    }

    gettimeofday(&start_time, NULL);

    #pragma omp parallel for
    for (long i = 0; i < VECTOR_SIZE; i++) {
        c[i] = a[i] + b[i];
    }

    gettimeofday(&end_time, NULL);

    // Cálculo do tempo
    long seconds = end_time.tv_sec - start_time.tv_sec;
    long micros = ((seconds * 1000000) + end_time.tv_usec) - (start_time.tv_usec);
    int num_threads = omp_get_max_threads();
    
    // Abre o arquivo CSV em modo "append" (adicionar ao final)
    FILE *csv_file = fopen(CSV_FILENAME, "a");
    if (csv_file == NULL) {
        fprintf(stderr, "Não foi possível abrir o arquivo CSV '%s'.\n", CSV_FILENAME);
        return 1;
    }

    // Se o arquivo estiver vazio, escreve o cabeçalho
    fseek(csv_file, 0, SEEK_END);
    if (ftell(csv_file) == 0) {
        fprintf(csv_file, "Threads,ExecutionTime_micros\n");
    }

    // Escreve a nova linha de dados
    fprintf(csv_file, "%d,%ld\n", num_threads, micros);
    fclose(csv_file);

    printf("Resultados para %d threads (Memory-Bound) salvos em %s\n", num_threads, CSV_FILENAME);

    free(a);
    free(b);
    free(c);

    return 0;
}
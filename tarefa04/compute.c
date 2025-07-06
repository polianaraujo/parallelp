#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <math.h>
#include <omp.h>
#include <unistd.h>

#define VECTOR_SIZE 20000000
#define INTENSIVE_ITERATIONS 50
#define CSV_FILENAME "compute_results.csv"

int main() {
    double *data;
    struct timeval start_time, end_time;

    // Alocação e inicialização
    data = (double*)malloc(sizeof(double) * VECTOR_SIZE);
    if (data == NULL) {
        fprintf(stderr, "Falha na alocação de memória.\n");
        return 1;
    }
    for (long i = 0; i < VECTOR_SIZE; i++) {
        data[i] = (double)i / (double)VECTOR_SIZE;
    }

    gettimeofday(&start_time, NULL);

    #pragma omp parallel for
    for (long i = 0; i < VECTOR_SIZE; i++) {
        double result = data[i];
        for (int j = 0; j < INTENSIVE_ITERATIONS; j++) {
            result = sin(result) * cos(result) + sqrt(fabs(result)) * pow(result, 1.2);
        }
        data[i] = result;
    }

    gettimeofday(&end_time, NULL);

    long seconds = end_time.tv_sec - start_time.tv_sec;
    long micros = ((seconds * 1000000) + end_time.tv_usec) - (start_time.tv_usec);
    int num_threads = omp_get_max_threads();

    // Abre o arquivo CSV em modo "append"
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

    printf("Resultados para %d threads (CPU-Bound) salvos em %s\n", num_threads, CSV_FILENAME);

    free(data);
    return 0;
}
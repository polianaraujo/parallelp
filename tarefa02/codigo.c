/*
Implemente três laços em C para investigar os efeitos do paralelismo ao nível de instrução (ILP): 1) inicialize um vetor com um cálculo simples; 2) some seus elementos de forma acumulativa, criando dependência entre as iterações; e 3) quebre essa dependência utilizando múltiplas variáveis. Compare o tempo de execução das versões compiladas com diferentes níveis de otimização (O0, O2, O3) e analise como o estilo do código e as dependências influenciam o desempenho.
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Aumentar N para que as diferenças de tempo sejam mais evidentes
#define N 200000000

// Laço 1: Inicializa o vetor com um cálculo simples (independente por iteração)
void initialize_array(double *arr) {
    for (int i = 0; i < N; i++) {
        arr[i] = (double)i * 1.1; // Cálculo independente, ideal para ILP
    }
}

// Laço 2: Soma sequencial com dependência de dados entre iterações
double sum_sequential(double *arr) {
    double sum = 0.0;
    // Cada iteração (sum += ...) depende do resultado da anterior.
    // Isso cria uma cadeia de dependência que limita o ILP.
    for (int i = 0; i < N; i++) {
        sum += arr[i];
    }
    return sum;
}

// Laço 3: Soma com dependência quebrada usando acumuladores múltiplos
double sum_parallelized(double *arr) {
    double sum1 = 0.0, sum2 = 0.0, sum3 = 0.0, sum4 = 0.0;
    // Quebra a cadeia de dependência usando 4 acumuladores independentes.
    // O processador pode executar as 4 somas de cada iteração em paralelo (ILP).
    for (int i = 0; i < N; i += 4) {
        sum1 += arr[i];
        sum2 += arr[i+1];
        sum3 += arr[i+2];
        sum4 += arr[i+3];
    }
    // Soma final dos resultados parciais
    return sum1 + sum2 + sum3 + sum4;
}

int main(int argc, char *argv[]) {
    // Valida se o nível de otimização foi passado como argumento
    if (argc < 2) {
        fprintf(stderr, "Uso: %s <NivelDeOtimizacao>\n", argv[0]);
        return 1;
    }
    char *optimization_level = argv[1];

    // Abre o arquivo CSV em modo de anexo (append)
    FILE *csv_file = fopen("results.csv", "a");
    if (csv_file == NULL) {
        perror("Erro ao abrir o arquivo CSV");
        return 1;
    }

    // Aloca memória para o vetor
    double *arr = (double*) malloc(N * sizeof(double));
    if (arr == NULL) {
        perror("Erro ao alocar memória");
        fclose(csv_file);
        return 1;
    }

    clock_t start, end;
    double cpu_time_used;

    // --- 1. Medir o tempo de initialize_array ---
    start = clock();
    initialize_array(arr);
    end = clock();
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    fprintf(csv_file, "%s,initialize_array,%.6f\n", optimization_level, cpu_time_used);
    printf("[%s] initialize_array: %.6f s\n", optimization_level, cpu_time_used);

    // --- 2. Medir o tempo de sum_sequential ---
    start = clock();
    double res_seq = sum_sequential(arr);
    end = clock();
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    fprintf(csv_file, "%s,sum_sequential,%.6f\n", optimization_level, cpu_time_used);
    printf("[%s] sum_sequential:   %.6f s (Resultado: %.2f)\n", optimization_level, cpu_time_used, res_seq);


    // --- 3. Medir o tempo de sum_parallelized ---
    start = clock();
    double res_par = sum_parallelized(arr);
    end = clock();
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    fprintf(csv_file, "%s,sum_parallelized,%.6f\n", optimization_level, cpu_time_used);
    printf("[%s] sum_parallelized: %.6f s (Resultado: %.2f)\n", optimization_level, cpu_time_used, res_par);

    // Libera recursos
    fclose(csv_file);
    free(arr);

    return 0;
}
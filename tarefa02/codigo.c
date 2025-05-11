/*
Implemente três laços em C para investigar os efeitos do paralelismo ao nível de instrução (ILP): 1) inicialize um vetor com um cálculo simples; 2) some seus elementos de forma acumulativa, criando dependência entre as iterações; e 3) quebre essa dependência utilizando múltiplas variáveis. Compare o tempo de execução das versões compiladas com diferentes níveis de otimização (O0, O2, O3) e analise como o estilo do código e as dependências influenciam o desempenho.
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 100000000

// Função para inicializar o vetor com um cálculo simples
void initialize_array(double *arr) {
    for (int i = 0; i < N; i++) {
        arr[i] = (double)i * 1.1; // Cada elemento recebe um valor baseado no índice
    }
}

// Função para calcular a soma sequencialmente (com dependência entre interações)
double sum_sequential(double *arr) {
    double sum = 0.0;
    for (int i = 0; i < N; i++) {
        sum += arr[i]; // Cada interação depende da anterior, limitando o paralelismo
    }
    return sum;
}

// Função para calcular a soma quebrando a dependência utilizando múltiplas variáveis
double sum_parallelized(double *arr) {
    double sum1 = 0.0, sum2 = 0.0, sum3 = 0.0, sum4 = 0.0;
    for (int i = 0; i < N; i += 4) { // Processa 4 elementos por vez
        sum1 += arr[i];
        sum2 += arr[i+1];
        sum3 += arr[i+2];
        sum4 += arr[i+3];
    }
    return sum1 + sum2 + sum3 + sum4; // Combina os resultados parciais
}

// Função para medir o tempo de execução de cada abordage
void measure_time(double (*func)(double*), double *arr, const char *label) {
    clock_t start, end;
    start = clock();            // Marca o tempo inicial
    double result = func(arr);  // Executa a função e armazena o resultado
    end = clock();              // Marca o tempo final
    printf("%s: Resultado = %f, Tempo = %f segundos\n", label, result, ((double)(end - start)) / CLOCKS_PER_SEC);
}

int main() {
    // Aloca dinamicamente memória para o vetor
    double *arr = (double*) malloc(N * sizeof(double));
    initialize_array(arr);  // Inicializa o vetor

    // Mede o tempo de execução para a soma sequencial e paralelizada
    measure_time(sum_sequential, arr, "Soma Sequencial");
    measure_time(sum_parallelized, arr, "Soma Paralelizada");

    free(arr);  // Libera memória alocada
    return 0;
}

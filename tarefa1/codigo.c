/* Duas versões da multiplicação de matriz por vetor (MxV) em C: ma com acesso à matriz por linhas (linha externa, coluna interna) e outra por colunas (coluna externa, linha interna). Meça o tempo de eecução de cada versão com uma função apropriada e execute testes com diferentes tamanhos de matriz. Identificque a partir de que tamanho os tempos passam a divergir significativamente e explique por que isso ocorre, relacionando usas observações com o uso da memória cache e o padrão de acesso à memória. */

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

// Função para multiplicar uma matriz (m x n) por um vetor de tamanho n
// Percorre a matriz linha por linha
void mat_vec_row_major(double *matrix, double *vector, double *result, int m, int n) {
    for (int i = 0; i < m; i++) {
        result[i] = 0.0; // Inicializa o resultado da linha
        for (int j = 0; j < n; j++) {
            result[i] += matrix[i * n + j] * vector[j];
        }
    }
}

// Função para multiplicar uma matriz (m x n) por um vetor de tamanho n
// Percorre a matriz coluna por coluna
void mat_vec_col_major(double *matrix, double *vector, double *result, int m, int n) {
    for (int i = 0; i < m; i++) {
        result[i] = 0.0; // Inicializa o vetor resultado
    }
    for (int j = 0; j < n; j++) {
        for (int i = 0; i < m; i++) {
            result[i] += matrix[i + j * m] * vector[j];
        }
    }
}

// Função para obter o tempo atual em microssegundos
long getTimeInMicroseconds() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000000 + tv.tv_usec;
}

int main() {
    // Definição dos tamanhos de matrizes a serem testadas
    int sizes[][2] = {
        {128, 256}, 
        {512, 1024}, 
        {1024, 2048}, 
        {2048, 4096},
        {4096, 8192},
        {8192, 16384}
    };
    int num_sizes = sizeof(sizes) / sizeof(sizes[0]); // Quantidade de tamanhos a testar

    for (int s = 0; s < num_sizes; s++) {
        int m = sizes[s][0];  // Número de linhas da matriz
        int n = sizes[s][1];  // Número de colunas da matriz
        printf("Tamanho da matriz: %d x %d\n", m, n);

        // Aloca memória para matriz, vetor e vetor resultado
        double *matrix = (double *) malloc(m * n * sizeof(double));
        double *vector = (double *) malloc(n * sizeof(double));  // Vetor de entrada
        double *result = (double *) malloc(m * sizeof(double));  // Vetor resultado

        // Inicializa a matriz e o vetor com valores aleatórios
        for (int i = 0; i < m * n; i++) {
            matrix[i] = rand() / (double) RAND_MAX;
        }
        for (int i = 0; i < n; i++) {
            vector[i] = rand() / (double) RAND_MAX;
        }

        long start, end;

        // Mede o tempo da multiplicacao linha por linha
        start = getTimeInMicroseconds();
        mat_vec_row_major(matrix, vector, result, m, n);
        end = getTimeInMicroseconds();
        printf("Linha por linha: %ld microssegundos\n", end - start);

        // Mede o tempo da multiplicacao coluna por coluna
        start = getTimeInMicroseconds();
        mat_vec_col_major(matrix, vector, result, m, n);
        end = getTimeInMicroseconds();
        printf("Coluna por coluna: %ld microssegundos\n", end - start);

        // Libera a memória alocada
        free(matrix);
        free(vector);
        free(result);
    }
    return 0;
}
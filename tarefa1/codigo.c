/* Duas versões da multiplicação de matriz por vetor (MxV) em C: ma com acesso à matriz por linhas (linha externa, coluna interna) e outra por colunas (coluna externa, linha interna). Meça o tempo de eecução de cada versão com uma função apropriada e execute testes com diferentes tamanhos de matriz. Identificque a partir de que tamanho os tempos passam a divergir significativamente e explique por que isso ocorre, relacionando usas observações com o uso da memória cache e o padrão de acesso à memória. */

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

// Função para multiplicação linha por linha (m x n)
void mat_vec_row_major(double *matrix, double *vector, double *result, int m, int n) {
    for (int i = 0; i < m; i++) {
        result[i] = 0.0;
        for (int j = 0; j < n; j++) {
            result[i] += matrix[i * n + j] * vector[j];
        }
    }
}

// Função para multiplicação coluna por coluna (m x n)
void mat_vec_col_major(double *matrix, double *vector, double *result, int m, int n) {
    for (int i = 0; i < m; i++) {
        result[i] = 0.0;
    }
    for (int j = 0; j < n; j++) {
        for (int i = 0; i < m; i++) {
            result[i] += matrix[i + j * m] * vector[j];
        }
    }
}

// Função para medir o tempo em microssegundos
long getTimeInMicroseconds() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000000 + tv.tv_usec;
}

int main() {
    // Definir tamanhos para testes: m x n (m linhas, n colunas)
    int sizes[][2] = {
        {128, 256}, 
        {512, 1024}, 
        {1024, 2048}, 
        {2048, 4096},
        {4096, 8192}
    };
    int num_sizes = sizeof(sizes) / sizeof(sizes[0]);

    for (int s = 0; s < num_sizes; s++) {
        int m = sizes[s][0];  // Número de linhas
        int n = sizes[s][1];  // Número de colunas
        printf("Tamanho da matriz: %d x %d\n", m, n);

        // Alocar memória para a matriz, vetor e resultado
        double *matrix = (double *) malloc(m * n * sizeof(double));
        double *vector = (double *) malloc(n * sizeof(double));  // Vetor de tamanho n
        double *result = (double *) malloc(m * sizeof(double));  // Resultado de tamanho m

        // Inicializa matriz e vetor com valores aleatórios
        for (int i = 0; i < m * n; i++) {
            matrix[i] = rand() / (double) RAND_MAX;
        }
        for (int i = 0; i < n; i++) {
            vector[i] = rand() / (double) RAND_MAX;
        }

        long start, end;

        // Teste da versão linha por linha
        start = getTimeInMicroseconds();
        mat_vec_row_major(matrix, vector, result, m, n);
        end = getTimeInMicroseconds();
        printf("Linha por linha: %ld microssegundos\n", end - start);

        // Exibir resultado (primeiros 10 valores)
        /* printf("Resultado (primeiros 10 valores): ");
        for (int i = 0; i < (m < 10 ? m : 10); i++) {
            printf("%.3f ", result[i]);
        }
        printf("\n"); */

        // Teste da versão coluna por coluna
        start = getTimeInMicroseconds();
        mat_vec_col_major(matrix, vector, result, m, n);
        end = getTimeInMicroseconds();
        printf("Coluna por coluna: %ld microssegundos\n", end - start);

        // Exibir resultado (primeiros 10 valores)
        /* printf("Resultado (primeiros 10 valores): ");
        for (int i = 0; i < (m < 10 ? m : 10); i++) {
            printf("%.3f ", result[i]);
        }
        printf("\n\n"); */

        free(matrix);
        free(vector);
        free(result);
    }
    return 0;
}
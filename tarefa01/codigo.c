/* Duas versões da multiplicação de matriz por vetor (MxV) em C: ma com acesso à matriz por linhas (linha externa, coluna interna) e outra por colunas (coluna externa, linha interna). Meça o tempo de eecução de cada versão com uma função apropriada e execute testes com diferentes tamanhos de matriz. Identificque a partir de que tamanho os tempos passam a divergir significativamente e explique por que isso ocorre, relacionando usas observações com o uso da memória cache e o padrão de acesso à memória. */

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

// Função para multiplicar uma matriz (N x N) por um vetor de tamanho N
// Percorre a matriz linha por linha (acesso sequencial e eficiente)
void mat_vec_row_major(double *matrix, double *vector, double *result, int N) {
    for (int i = 0; i < N; i++) {
        result[i] = 0.0;
        for (int j = 0; j < N; j++) {
            result[i] += matrix[i * N + j] * vector[j];
        }
    }
}

// Função para multiplicar uma matriz (N x N) por um vetor de tamanho N
// Percorre a matriz coluna por coluna (acesso não sequencial e ineficiente)
void mat_vec_col_major(double *matrix, double *vector, double *result, int N) {
    for (int i = 0; i < N; i++) {
        result[i] = 0.0;
    }
    for (int j = 0; j < N; j++) {
        for (int i = 0; i < N; i++) {
            result[i] += matrix[i * N + j] * vector[j];
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
    // Abre o arquivo CSV para escrita
    FILE *csv_file = fopen("resultados.csv", "w");
    if (csv_file == NULL) {
        printf("Erro ao abrir o arquivo resultados.csv\n");
        return 1;
    }

    // Escreve o cabeçalho do CSV
    fprintf(csv_file, "Tamanho_N,Tempo_Linha_us,Tempo_Coluna_us\n");

    int sizes[] = {32, 64, 128, 256, 512, 1024};
    int num_sizes = sizeof(sizes) / sizeof(sizes[0]);

    for (int s = 0; s < num_sizes; s++) {
        int N = sizes[s];
        printf("Processando matriz: %d x %d...\n", N, N);

        double *matrix = (double *) malloc(N * N * sizeof(double));
        double *vector = (double *) malloc(N * sizeof(double));
        double *result = (double *) malloc(N * sizeof(double));

        for (int i = 0; i < N * N; i++) {
            matrix[i] = rand() / (double) RAND_MAX;
        }
        for (int i = 0; i < N; i++) {
            vector[i] = rand() / (double) RAND_MAX;
        }

        long start_row, end_row, time_row;
        long start_col, end_col, time_col;

        start_row = getTimeInMicroseconds();
        mat_vec_row_major(matrix, vector, result, N);
        end_row = getTimeInMicroseconds();
        time_row = end_row - start_row;

        start_col = getTimeInMicroseconds();
        mat_vec_col_major(matrix, vector, result, N);
        end_col = getTimeInMicroseconds();
        time_col = end_col - start_col;

        // Escreve os resultados da iteração atual no arquivo CSV
        fprintf(csv_file, "%d,%ld,%ld\n", N, time_row, time_col);

        free(matrix);
        free(vector);
        free(result);
    }

    // Fecha o arquivo
    fclose(csv_file);
    printf("Resultados salvos em resultados.csv\n");

    return 0;
}
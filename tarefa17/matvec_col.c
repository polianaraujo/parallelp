#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    int rank, size;
    int M = 8;  // número de linhas
    int N = 8;  // número de colunas

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (N % size != 0) {
        if (rank == 0)
            printf("Número de colunas (%d) deve ser divisível pelo número de processos (%d).\n", N, size);
        MPI_Finalize();
        return EXIT_FAILURE;
    }

    int local_cols = N / size;
    double *A = NULL, *x = NULL, *y = calloc(M, sizeof(double));

    if (rank == 0) {
        A = malloc(M * N * sizeof(double));
        x = malloc(N * sizeof(double));

        // Inicializa A e x
        for (int i = 0; i < M; i++)
            for (int j = 0; j < N; j++)
                A[i * N + j] = 1.0; // exemplo simples

        for (int j = 0; j < N; j++)
            x[j] = 1.0;
    }

    double *local_A = malloc(M * local_cols * sizeof(double));
    double *local_x = malloc(local_cols * sizeof(double));
    double *local_y = calloc(M, sizeof(double)); // contribuição parcial de y

    // Criação do tipo derivado para colunas
    MPI_Datatype column_type, resized_column_type;
    MPI_Type_vector(M, local_cols, N, MPI_DOUBLE, &column_type);
    MPI_Type_create_resized(column_type, 0, local_cols * sizeof(double), &resized_column_type);
    MPI_Type_commit(&resized_column_type);

    // Scatter das colunas da matriz A
    MPI_Scatter(A, 1, resized_column_type, local_A, M * local_cols, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // Scatter manual do vetor x correspondente às colunas
    if (rank == 0)
        for (int i = 0; i < size; i++)
            MPI_Send(x + i * local_cols, local_cols, MPI_DOUBLE, i, 0, MPI_COMM_WORLD);
    MPI_Recv(local_x, local_cols, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    // Cálculo da contribuição local para o vetor y
    for (int i = 0; i < M; i++)
        for (int j = 0; j < local_cols; j++)
            local_y[i] += local_A[i * local_cols + j] * local_x[j];

    // Redução (soma) para o processo 0
    MPI_Reduce(local_y, y, M, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    // Impressão do resultado no processo 0
    if (rank == 0) {
        printf("Resultado final y:\n");
        printf("y = [");
        for (int i = 0; i < M; i++)
            printf("%.2f ", y[i]);
        printf("]\n");
    }

    free(local_A);
    free(local_x);
    free(local_y);
    if (rank == 0) {
        free(A);
        free(x);
        free(y);
    }

    MPI_Type_free(&column_type);
    MPI_Type_free(&resized_column_type);
    MPI_Finalize();
    return 0;
}

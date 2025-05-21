#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

void print_vector(double *vec, int size, const char *name) {
    printf("%s = [", name);
    for (int i = 0; i < size; i++)
        printf("%.2f ", vec[i]);
    printf("]\n");
}

int main(int argc, char *argv[]) {
    int rank, size;
    int M = 8;  // número de linhas
    int N = 4;  // número de colunas

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (M % size != 0) {
        if (rank == 0) printf("Erro: M deve ser divisível pelo número de processos.\n");
        MPI_Finalize();
        return -1;
    }

    int local_rows = M / size;

    double *A = NULL;
    double *x = (double *)malloc(N * sizeof(double));
    double *local_A = (double *)malloc(local_rows * N * sizeof(double));
    double *local_y = (double *)malloc(local_rows * sizeof(double));
    double *y = NULL;

    if (rank == 0) {
        A = (double *)malloc(M * N * sizeof(double));
        y = (double *)malloc(M * sizeof(double));

        // Inicialização de A e x
        for (int i = 0; i < M * N; i++)
            A[i] = 1.0;  // ou use rand() para aleatório
        for (int i = 0; i < N; i++)
            x[i] = 1.0;
    }

    double start_time = MPI_Wtime();

    MPI_Scatter(A, local_rows * N, MPI_DOUBLE, local_A, local_rows * N, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Bcast(x, N, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // Produto local: local_y = local_A * x
    for (int i = 0; i < local_rows; i++) {
        local_y[i] = 0.0;
        for (int j = 0; j < N; j++) {
            local_y[i] += local_A[i * N + j] * x[j];
        }
    }

    MPI_Gather(local_y, local_rows, MPI_DOUBLE, y, local_rows, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    double end_time = MPI_Wtime();

    if (rank == 0) {
        printf("Resultado final y:\n");
        print_vector(y, M, "y");
        printf("Tempo total: %f segundos\n", end_time - start_time);
        free(A);
        free(y);
    }

    free(x);
    free(local_A);
    free(local_y);

    MPI_Finalize();
    return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main(int argc, char *argv[]) {
    int rank, size, M, N;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (argc != 3) {
        if (rank == 0)
            printf("Uso: %s <M> <N>\n", argv[0]);
        MPI_Finalize();
        return -1;
    }

    M = atoi(argv[1]);
    N = atoi(argv[2]);

    if (M % size != 0) {
        if (rank == 0)
            printf("Erro: M (%d) deve ser divisível pelo número de processos (%d).\n", M, size);
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

        for (int i = 0; i < M * N; i++) A[i] = 1.0;
        for (int i = 0; i < N; i++) x[i] = 1.0;
    }

    double start = MPI_Wtime();

    MPI_Scatter(A, local_rows * N, MPI_DOUBLE, local_A, local_rows * N, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Bcast(x, N, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    for (int i = 0; i < local_rows; i++) {
        local_y[i] = 0.0;
        for (int j = 0; j < N; j++) {
            local_y[i] += local_A[i * N + j] * x[j];
        }
    }

    MPI_Gather(local_y, local_rows, MPI_DOUBLE, y, local_rows, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    double end = MPI_Wtime();
    double tempo = end - start;

    if (rank == 0) {
        FILE *fp = fopen("resultados.csv", "a");
        if (fp != NULL) {
            fprintf(fp, "%d,%d,%d,%.6f\n", M, N, size, tempo);
            fclose(fp);
        } else {
            fprintf(stderr, "Erro ao abrir arquivo CSV.\n");
        }

        free(A);
        free(y);
    }

    free(x);
    free(local_A);
    free(local_y);
    MPI_Finalize();
    return 0;
}

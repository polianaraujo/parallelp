#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main(int argc, char *argv[]) {
    int rank, size, M, N; // rank: ID do processo; size: número total de processos; M, N: dimensões da matriz

    // Inicializa o ambiente MPI.
    MPI_Init(&argc, &argv);

    // Rank do processo atual dentro do comunicador MPI_COMM_WORLD.
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // Obtém o número total de processos no comunicador MPI_COMM_WORLD.
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Validação de argumentos e divisibilidade

    // Verifica se o número de argumentos está correto (nome do programa + M + N)
    if (argc != 3) {
        if (rank == 0)
            printf("Uso: %s <M> <N>\n", argv[0]);
        MPI_Finalize();
        return -1;
    }

    M = atoi(argv[1]);
    N = atoi(argv[2]);

    // Garante que o número de colunas (N) é divisível pelo número de processos (size), para distribuição uniforme de colunas
    if (N % size != 0) {
        if (rank == 0)  // // Apenas o processo mestre imprime a mensagem de uso
            printf("Erro: N (%d) deve ser divisível pelo número de processos (%d).\n", N, size);
        MPI_Finalize();
        return -1;
    }

    // Número de colunas que cada processo irá manipular
    int local_cols = N / size;

    double *A = NULL;
    double *x = NULL;
    double *local_A = (double *)malloc(M * local_cols * sizeof(double));
    double *local_x = (double *)malloc(local_cols * sizeof(double));
    double *local_y = (double *)calloc(M, sizeof(double));  // contribuição parcial
    double *y = NULL;

    if (rank == 0) {
        A = (double *)malloc(M * N * sizeof(double));
        x = (double *)malloc(N * sizeof(double));
        y = (double *)malloc(M * sizeof(double));

        for (int i = 0; i < M * N; i++) A[i] = 1.0;
        for (int i = 0; i < N; i++) x[i] = 1.0;
    }

    double start = MPI_Wtime();

    // Criar tipo derivado para colunas intercaladas
    MPI_Datatype column_type, resized_col_type;
    MPI_Type_vector(M, local_cols, N, MPI_DOUBLE, &column_type);
    MPI_Type_create_resized(column_type, 0, local_cols * sizeof(double), &resized_col_type);
    MPI_Type_commit(&resized_col_type);

    // Distribuir blocos de colunas da matriz A
    MPI_Scatter(A, 1, resized_col_type, local_A, M * local_cols, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // Distribuir pedaço correspondente de x
    if (rank == 0) {
        for (int i = 0; i < size; i++) {
            if (i == 0)
                for (int j = 0; j < local_cols; j++) local_x[j] = x[j];
            else
                MPI_Send(&x[i * local_cols], local_cols, MPI_DOUBLE, i, 0, MPI_COMM_WORLD);
        }
    } else {
        MPI_Recv(local_x, local_cols, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    // Cada processo calcula sua contribuição para o vetor y
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < local_cols; j++) {
            local_y[i] += local_A[i * local_cols + j] * local_x[j];
        }
    }

    // Reduzir as contribuições parciais de todos os processos para y em rank 0
    MPI_Reduce(local_y, y, M, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    double end = MPI_Wtime();
    double tempo = end - start;

    // GRavação de resultados e limpeza
    if (rank == 0) {
        FILE *fp = fopen("resultados.csv", "a");
        if (fp != NULL) {
            fprintf(fp, "%d,%d,%d,%.6f\n", M, N, size, tempo);
            fclose(fp);
        } else {
            fprintf(stderr, "Erro ao abrir arquivo CSV.\n");
        }

        free(A);
        free(x);
        free(y);
    }

    free(local_A);
    free(local_x);
    free(local_y);
    MPI_Type_free(&column_type);
    MPI_Type_free(&resized_col_type);
    MPI_Finalize();
    return 0;
}

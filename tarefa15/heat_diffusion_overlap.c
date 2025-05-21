#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define N 1000
#define STEPS 1000
#define ALPHA 0.01
#define LEFT_TEMP 100.0
#define RIGHT_TEMP 50.0

void exchange_borders_test(double* u, double *u_new, int rank, int size, int local_n) {
    MPI_Request reqs[4];
    int flag[4] = {0, 0, 0, 0};

    if (rank > 0) {
        MPI_Irecv(&u[0], 1, MPI_DOUBLE, rank - 1, 0, MPI_COMM_WORLD, &reqs[0]);
        MPI_Isend(&u[1], 1, MPI_DOUBLE, rank - 1, 1, MPI_COMM_WORLD, &reqs[1]);
    } else {
        u[0] = LEFT_TEMP;
        reqs[0] = reqs[1] = MPI_REQUEST_NULL;
        flag[0] = flag[1] = 1;
    }

    if (rank < size - 1) {
        MPI_Irecv(&u[local_n - 1], 1, MPI_DOUBLE, rank + 1, 1, MPI_COMM_WORLD, &reqs[2]);
        MPI_Isend(&u[local_n - 2], 1, MPI_DOUBLE, rank + 1, 0, MPI_COMM_WORLD, &reqs[3]);
    } else {
        u[local_n - 1] = RIGHT_TEMP;
        reqs[2] = reqs[3] = MPI_REQUEST_NULL;
        flag[2] = flag[3] = 1;
    }

    // Atualiza pontos internos enquanto aguarda bordas
    while (!flag[0] || !flag[2]) {
        if (!flag[0]) MPI_Test(&reqs[0], &flag[0], MPI_STATUS_IGNORE);
        if (!flag[2]) MPI_Test(&reqs[2], &flag[2], MPI_STATUS_IGNORE);

        for (int i = 2; i < local_n - 2; i++) // internos seguros
            u_new[i] = u[i] + ALPHA * (u[i - 1] - 2 * u[i] + u[i + 1]);
    }

    MPI_Wait(&reqs[1], MPI_STATUS_IGNORE);
    MPI_Wait(&reqs[3], MPI_STATUS_IGNORE);

    // Atualiza bordas seguras
    u_new[1] = u[1] + ALPHA * (u[0] - 2 * u[1] + u[2]);
    u_new[local_n - 2] = u[local_n - 2] + ALPHA * (u[local_n - 3] - 2 * u[local_n - 2] + u[local_n - 1]);
}

int main(int argc, char** argv) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int local_n = N / size + 2; // inclui as bordas fantasma
    double* u = calloc(local_n, sizeof(double));
    double* u_new = calloc(local_n, sizeof(double));

    double start = MPI_Wtime();
    for (int t = 0; t < STEPS; t++) {
        exchange_borders_test(u, rank, size, local_n);
        double* temp = u;
        u = u_new;
        u_new = temp;
    }
    double end = MPI_Wtime();

    if (rank == 0)
        printf("Tempo (MPI_Test + Overlap): %.4f segundos\n", end - start);
    
    free(u); free(u_new);
    MPI_Finalize();
    return 0;
}

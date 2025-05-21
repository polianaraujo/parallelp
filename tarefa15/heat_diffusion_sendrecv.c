#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define N 1000
#define STEPS 1000
#define ALPHA 0.01
#define LEFT_TEMP 100.0
#define RIGHT_TEMP 50.0

void exchange_borders(double* u, int rank, int size, int local_n) {
    if (rank > 0) // Envia para esquerda, recebe da esquerda
        MPI_Sendrecv(&u[1], 1, MPI_DOUBLE, rank - 1, 0,
                     &u[0], 1, MPI_DOUBLE, rank - 1, 0,
                     MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    else
        u[0] = LEFT_TEMP;

    if (rank < size - 1) // Envia para direita, recebe da direita
        MPI_Sendrecv(&u[local_n - 2], 1, MPI_DOUBLE, rank + 1, 0,
                     &u[local_n - 1], 1, MPI_DOUBLE, rank + 1, 0,
                     MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    else
        u[local_n - 1] = RIGHT_TEMP;
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
        exchange_borders(u, rank, size, local_n);

        for (int i = 1; i < local_n - 1; i++)
            u_new[i] = u[i] + ALPHA * (u[i - 1] - 2 * u[i] + u[i + 1]);

        double* temp = u;
        u = u_new;
        u_new = temp;
    }
    double end = MPI_Wtime();

    if (rank == 0)
        printf("Tempo (Send/Recv): %.4f segundos\n", end - start);

    free(u); free(u_new);
    MPI_Finalize();
    return 0;
}

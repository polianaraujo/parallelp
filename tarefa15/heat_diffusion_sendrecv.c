#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define N 100000          // Total de pontos na barra
#define STEPS 10000      // Passos de tempo
#define ALPHA 0.01f     // Difusividade térmica

void exchange_boundaries(double *u, int local_n, int rank, int size, MPI_Comm comm) {
    if (rank > 0)
        MPI_Send(&u[1], 1, MPI_DOUBLE, rank - 1, 0, comm);
    if (rank < size - 1)
        MPI_Send(&u[local_n], 1, MPI_DOUBLE, rank + 1, 1, comm);

    if (rank < size - 1)
        MPI_Recv(&u[local_n + 1], 1, MPI_DOUBLE, rank + 1, 0, comm, MPI_STATUS_IGNORE);
    if (rank > 0)
        MPI_Recv(&u[0], 1, MPI_DOUBLE, rank - 1, 1, comm, MPI_STATUS_IGNORE);
}

int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm comm = MPI_COMM_WORLD;
    MPI_Comm_rank(comm, &rank);
    MPI_Comm_size(comm, &size);

    int local_n = N / size;
    double *u = calloc(local_n + 2, sizeof(double));
    double *u_new = calloc(local_n + 2, sizeof(double));

    // Condições iniciais
    for (int i = 1; i <= local_n; ++i)
        u[i] = (rank * local_n + i == N / 2) ? 100.0 : 0.0;

    double start = MPI_Wtime();
    for (int step = 0; step < STEPS; ++step) {
        exchange_boundaries(u, local_n, rank, size, comm);

        for (int i = 1; i <= local_n; ++i)
            u_new[i] = u[i] + ALPHA * (u[i - 1] - 2 * u[i] + u[i + 1]);

        double *tmp = u;
        u = u_new;
        u_new = tmp;
    }
    double end = MPI_Wtime();

    if (rank == 0)
        printf("Tempo (Send/Recv): %.6f s\n", end - start);

    free(u);
    free(u_new);
    MPI_Finalize();
    return 0;
}

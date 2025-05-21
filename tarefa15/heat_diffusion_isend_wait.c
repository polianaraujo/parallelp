#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define N 1000
#define STEPS 1000
#define ALPHA 0.01f

void exchange_boundaries(double *u, int local_n, int rank, int size, MPI_Comm comm) {
    MPI_Request reqs[4];

    if (rank > 0)
        MPI_Irecv(&u[0], 1, MPI_DOUBLE, rank - 1, 1, comm, &reqs[0]);
    else
        reqs[0] = MPI_REQUEST_NULL;

    if (rank < size - 1)
        MPI_Irecv(&u[local_n + 1], 1, MPI_DOUBLE, rank + 1, 0, comm, &reqs[1]);
    else
        reqs[1] = MPI_REQUEST_NULL;

    if (rank > 0)
        MPI_Isend(&u[1], 1, MPI_DOUBLE, rank - 1, 0, comm, &reqs[2]);
    else
        reqs[2] = MPI_REQUEST_NULL;

    if (rank < size - 1)
        MPI_Isend(&u[local_n], 1, MPI_DOUBLE, rank + 1, 1, comm, &reqs[3]);
    else
        reqs[3] = MPI_REQUEST_NULL;

    MPI_Waitall(4, reqs, MPI_STATUSES_IGNORE);
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
        printf("Tempo (Isend/Irecv + Wait): %.6f s\n", end - start);

    free(u);
    free(u_new);
    MPI_Finalize();
    return 0;
}

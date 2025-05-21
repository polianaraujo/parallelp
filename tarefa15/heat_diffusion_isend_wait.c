// ... mesmo includes e defines

void exchange_borders_async(double* u, int rank, int size, int local_n) {
    MPI_Request reqs[4];

    if (rank > 0) {
        MPI_Irecv(&u[0], 1, MPI_DOUBLE, rank - 1, 0, MPI_COMM_WORLD, &reqs[0]);
        MPI_Isend(&u[1], 1, MPI_DOUBLE, rank - 1, 1, MPI_COMM_WORLD, &reqs[1]);
    } else {
        u[0] = LEFT_TEMP;
        reqs[0] = reqs[1] = MPI_REQUEST_NULL;
    }

    if (rank < size - 1) {
        MPI_Irecv(&u[local_n - 1], 1, MPI_DOUBLE, rank + 1, 1, MPI_COMM_WORLD, &reqs[2]);
        MPI_Isend(&u[local_n - 2], 1, MPI_DOUBLE, rank + 1, 0, MPI_COMM_WORLD, &reqs[3]);
    } else {
        u[local_n - 1] = RIGHT_TEMP;
        reqs[2] = reqs[3] = MPI_REQUEST_NULL;
    }

    MPI_Waitall(4, reqs, MPI_STATUSES_IGNORE);
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
        exchange_borders_async(u, rank, size, local_n);

        for (int i = 1; i < local_n - 1; i++)
            u_new[i] = u[i] + ALPHA * (u[i - 1] - 2 * u[i] + u[i + 1]);

        double* temp = u;
        u = u_new;
        u_new = temp;
    }
    double end = MPI_Wtime();

    if (rank == 0)
        printf("Tempo (Isend/Irecv + Wait): %.4f segundos\n", end - start);

    free(u); free(u_new);
    MPI_Finalize();
    return 0;
}

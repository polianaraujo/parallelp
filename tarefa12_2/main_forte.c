#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>

#define N 128
#define NSTEPS 100
#define DT 0.01f
#define DX 1.0f
#define VISC 0.1f

float u[N][N][N], u_new[N][N][N];

void initialize() {
    memset(u, 0, sizeof(u));
    int cx = N / 2, cy = N / 2, cz = N / 2;
    u[cx][cy][cz] = 1.0f; // Pequena perturbação central
}

void simulate_step() {
    #pragma omp parallel for collapse(3) schedule(static)
    for (int i = 1; i < N - 1; i++) {
        for (int j = 1; j < N - 1; j++) {
            for (int k = 1; k < N - 1; k++) {
                float laplacian = (u[i + 1][j][k] + u[i - 1][j][k] +
                                   u[i][j + 1][k] + u[i][j - 1][k] +
                                   u[i][j][k + 1] + u[i][j][k - 1] -
                                   6.0f * u[i][j][k]) / (DX * DX);
                u_new[i][j][k] = u[i][j][k] + DT * VISC * laplacian;
            }
        }
    }
    memcpy(u, u_new, sizeof(u));
}

int main(int argc, char* argv[]) {
    if (argc < 4) {
        fprintf(stderr, "Uso: %s <schedule> <collapse> <n_threads>\n", argv[0]);
        return 1;
    }

    char* schedule = argv[1];
    int collapse_level = atoi(argv[2]);
    int n_threads = atoi(argv[3]);
    omp_set_num_threads(n_threads);
    ...


    omp_sched_t omp_schedule;
    if (strcmp(schedule, "static") == 0) {
        omp_schedule = omp_sched_static;
    } else if (strcmp(schedule, "dynamic") == 0) {
        omp_schedule = omp_sched_dynamic;
    } else if (strcmp(schedule, "guided") == 0) {
        omp_schedule = omp_sched_guided;
    } else {
        fprintf(stderr, "Schedule inválido. Use static, dynamic ou guided.\n");
        return 1;
    }
    omp_set_schedule(omp_schedule, 0);

    int num_threads = omp_get_max_threads();
    initialize();

    double start = omp_get_wtime();
    for (int step = 0; step < NSTEPS; step++) {
        if (collapse_level == 1) {
            #pragma omp parallel for collapse(1) schedule(runtime)
            for (int i = 1; i < N - 1; i++)
                for (int j = 1; j < N - 1; j++)
                    for (int k = 1; k < N - 1; k++) {
                        float laplacian = (u[i + 1][j][k] + u[i - 1][j][k] +
                                           u[i][j + 1][k] + u[i][j - 1][k] +
                                           u[i][j][k + 1] + u[i][j][k - 1] -
                                           6.0f * u[i][j][k]) / (DX * DX);
                        u_new[i][j][k] = u[i][j][k] + DT * VISC * laplacian;
                    }
        } else if (collapse_level == 2) {
            #pragma omp parallel for collapse(2) schedule(runtime)
            for (int i = 1; i < N - 1; i++)
                for (int j = 1; j < N - 1; j++)
                    for (int k = 1; k < N - 1; k++) {
                        float laplacian = (u[i + 1][j][k] + u[i - 1][j][k] +
                                           u[i][j + 1][k] + u[i][j - 1][k] +
                                           u[i][j][k + 1] + u[i][j][k - 1] -
                                           6.0f * u[i][j][k]) / (DX * DX);
                        u_new[i][j][k] = u[i][j][k] + DT * VISC * laplacian;
                    }
        } else {
            #pragma omp parallel for collapse(3) schedule(runtime)
            for (int i = 1; i < N - 1; i++)
                for (int j = 1; j < N - 1; j++)
                    for (int k = 1; k < N - 1; k++) {
                        float laplacian = (u[i + 1][j][k] + u[i - 1][j][k] +
                                           u[i][j + 1][k] + u[i][j - 1][k] +
                                           u[i][j][k + 1] + u[i][j][k - 1] -
                                           6.0f * u[i][j][k]) / (DX * DX);
                        u_new[i][j][k] = u[i][j][k] + DT * VISC * laplacian;
                    }
        }
        memcpy(u, u_new, sizeof(u));
    }
    double end = omp_get_wtime();
    double elapsed = end - start;

    // Salva resultados no CSV
    FILE* f = fopen("resultados.csv", "a");
    if (f) {
        printf("%s,%d,%s,%d,%d,%.6f\n", tipo_execucao, n_threads, schedule, collapse_level, N, elapsed);

        fclose(f);
    } else {
        perror("Erro ao abrir resultados.csv");
    }

    printf("Threads: %d\n", num_threads);
    printf("Schedule: %s | Collapse: %d | Tempo: %.6f s\n", schedule, collapse_level, elapsed);
    return 0;
}

//gcc -fopenmp -O3 navier_stokes_visc.c -o simulador

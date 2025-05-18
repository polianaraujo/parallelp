#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>

float ***alloc_3d(int N) {
    float ***array = malloc(N * sizeof(float **));
    for (int i = 0; i < N; i++) {
        array[i] = malloc(N * sizeof(float *));
        for (int j = 0; j < N; j++) {
            array[i][j] = calloc(N, sizeof(float));
        }
    }
    return array;
}

void free_3d(float ***array, int N) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            free(array[i][j]);
        }
        free(array[i]);
    }
    free(array);
}

void initialize(float ***u, int N) {
    int cx = N / 2, cy = N / 2, cz = N / 2;
    u[cx][cy][cz] = 1.0f;
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("Uso: %s <schedule: static|dynamic|guided> <collapse: 1|2|3> <N>\n", argv[0]);
        return 1;
    }

    char *schedule = argv[1];
    int collapse_level = atoi(argv[2]);
    int N = atoi(argv[3]);
    int NSTEPS = 100;
    float DT = 0.01f, DX = 1.0f, VISC = 0.1f;

    float ***u = alloc_3d(N);
    float ***u_new = alloc_3d(N);

    initialize(u, N);

    double start = omp_get_wtime();

    for (int step = 0; step < NSTEPS; step++) {
        if (collapse_level == 1) {
            #pragma omp parallel for schedule(static) collapse(1)
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
        } else if (collapse_level == 2) {
            #pragma omp parallel for schedule(static) collapse(2)
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
        } else {
            #pragma omp parallel for schedule(static) collapse(3)
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
        }

        // Troca os ponteiros
        float ***temp = u;
        u = u_new;
        u_new = temp;
    }

    double end = omp_get_wtime();
    double elapsed = end - start;
    int n_threads = omp_get_max_threads();

    printf("%d,%s,%d,%d,%.6f\n", n_threads, schedule, collapse_level, N, elapsed);

    free_3d(u, N);
    free_3d(u_new, N);
    return 0;
}


//gcc -fopenmp -O3 main_forte.c -o main_forte

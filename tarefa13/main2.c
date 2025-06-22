#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>

#define N 128
#define NSTEPS 100
#define DT 0.01f
#define DX 1.0f
#define VISC 0.1f

// Array de velocidade e da velocidade no próximo passo
float u[N][N][N], u_new[N][N][N];

void initialize() {
    memset(u, 0, sizeof(u));    // Inicia array com zeros e calcula seu tamanho (em bytes)
    int cx = N / 2, cy = N / 2, cz = N / 2; // Coordenadas do centro da grade
    u[cx][cy][cz] = 1.0f;   // Ponto central inicial de 1
}

void simulate() {
    for (int step = 0; step < NSTEPS; step++) {
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
}

int main() {
    int num_threads = omp_get_max_threads();
    double start = omp_get_wtime();
    initialize();
    simulate();
    double end = omp_get_wtime();

    printf("Threads: %d\n", num_threads);
    printf("Tempo: %.6f segundos\n", end - start);
    return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
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
    u[cx][cy][cz] = 1.0f;
}

void run_simulation(const char* affinity_type, int num_threads) {
    initialize();
    
    // Configuração simplificada de afinidade (versão portável)
    if (strcmp(affinity_type, "none") != 0) {
        printf("Aviso: Afinidade '%s' solicitada mas não configurada (OpenMP pode não suportar)\n", affinity_type);
    }
    
    omp_set_num_threads(num_threads);
    double start = omp_get_wtime();

    #pragma omp parallel
    {
        #pragma omp for collapse(3) schedule(static, 1)
        for (int i = 1; i < N - 1; i++)
            for (int j = 1; j < N - 1; j++)
                for (int k = 1; k < N - 1; k++)
                    u_new[i][j][k] = 0.0f;
    }

    for (int step = 0; step < NSTEPS; step++) {
        #pragma omp parallel
        {
            #pragma omp for collapse(3) schedule(guided, 1024)
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
    
    printf("\"%s\",%d,%.6f\n", affinity_type, num_threads, elapsed);
}

int main() {
    printf("affinity_type,num_threads,execution_time\n");
    
    const char* affinity_types[] = {"none", "compact", "scatter", "balanced"};
    int max_threads = omp_get_max_threads
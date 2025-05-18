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
    u[cx][cy][cz] = 1.0f;
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
    printf("affinity_type,num_threads,execution_time\n");

    const char* affinities[] = {"none", "close", "spread"};
    int max_threads = omp_get_max_threads();

    for (int a = 0; a < 3; a++) {
        const char* affinity = affinities[a];
        for (int t = 1; t <= max_threads; t++) {
            // Set variáveis de ambiente programaticamente
            if (strcmp(affinity, "none") == 0) {
                omp_set_proc_bind(omp_proc_bind_false);
            } else if (strcmp(affinity, "close") == 0) {
                omp_set_proc_bind(omp_proc_bind_close);
            } else if (strcmp(affinity, "spread") == 0) {
                omp_set_proc_bind(omp_proc_bind_spread);
            }

            omp_set_num_threads(t);
            initialize();

            double start = omp_get_wtime();
            simulate();
            double end = omp_get_wtime();

            printf("%s,%d,%.6f\n", affinity, t, end - start);
        }
    }

    return 0;
}

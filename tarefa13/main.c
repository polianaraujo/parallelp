// Utilizando estabilidade forte
// Tamanho permanece fixo enquanto varia o número de threads, para observar como o desempenho escala

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>
#include <string.h>

#define NSTEPS 1000
#define DT 0.01f
#define VISCOSITY 0.1f

void initialize_field(float **field, int nx, int ny) {
    for (int i = 0; i < nx; i++) {
        for (int j = 0; j < ny; j++) {
            field[i][j] = 0.0f;
        }
    }
    // Pequena perturbação no centro
    field[nx / 2][ny / 2] = 1.0f;
}

void run_simulation_forte(int num_threads, int nx, int ny, const char *schedule_type, int chunk_size) {
    float **field = malloc(nx * sizeof(float*));
    float **next_field = malloc(nx * sizeof(float*));
    for (int i = 0; i < nx; i++) {
        field[i] = malloc(ny * sizeof(float));
        next_field[i] = malloc(ny * sizeof(float));
    }

    omp_set_num_threads(num_threads);

    if (strcmp(schedule_type, "static") == 0)
        omp_set_schedule(omp_sched_static, chunk_size);
    else if (strcmp(schedule_type, "dynamic") == 0)
        omp_set_schedule(omp_sched_dynamic, chunk_size);
    else if (strcmp(schedule_type, "guided") == 0)
        omp_set_schedule(omp_sched_guided, chunk_size);

    // Inicializar campo
    initialize_field(field, nx, ny);

    double start_time = omp_get_wtime();

    for (int step = 0; step < NSTEPS; step++) {
        #pragma omp parallel for collapse(2) schedule(runtime)
        for (int i = 1; i < nx - 1; i++) {
            for (int j = 1; j < ny - 1; j++) {
                float laplacian = field[i+1][j] + field[i-1][j] +
                                  field[i][j+1] + field[i][j-1] -
                                  4.0f * field[i][j];
                next_field[i][j] = field[i][j] + VISCOSITY * DT * laplacian;
            }
        }

        // Trocar ponteiros
        float **temp = field;
        field = next_field;
        next_field = temp;
    }

    double end_time = omp_get_wtime();
    double exec_time = end_time - start_time;

    float center_value = field[nx/2][ny/2];
    float sum = 0.0f;
    for (int i = 0; i < nx; i++)
        for (int j = 0; j < ny; j++)
            sum += field[i][j];

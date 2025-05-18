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

void run_simulation_forte(int num_threads, int nx, int ny, const char *schedule_type, int chunk_size, const char *affinity_type)
{
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
    float avg_value = sum / (nx * ny);

    FILE *file = fopen("results_escalabilidade_forte.csv", "a");
    if (file) {
        fprintf(file, "%d,%d,%d,%s,%d,%s,%.5f,%.5f,%.5f\n",
        num_threads, nx, ny, schedule_type, chunk_size, affinity_type,
        exec_time, center_value, avg_value);

        fclose(file);
    }

    for (int i = 0; i < nx; i++) {
        free(field[i]);
        free(next_field[i]);
    }
    free(field);
    free(next_field);
}

int main(int argc, char *argv[]) {
    const char *affinity_type = (argc > 1) ? argv[1] : "undefined";

    FILE *file = fopen("results_escalabilidade_forte.csv", "a");
    if (!file) {
        fprintf(stderr, "Erro ao abrir arquivo de resultados.\n");
        return 1;
    }
    fclose(file);

    int threads[] = {1, 2, 4, 8, 16};
    const char *schedules[] = {"static"}; // pode adicionar "dynamic", "guided"
    int chunk_sizes[] = {4};              // pode adicionar mais tamanhos

    int nx = 100;
    int ny = 100;

    for (int t = 0; t < 5; t++) {
        int num_threads = threads[t];
        for (int s = 0; s < 1; s++) {
            for (int c = 0; c < 1; c++) {
                run_simulation_forte(num_threads, nx, ny, schedules[s], chunk_sizes[c], affinity_type);
            }
        }
    }

    return 0;
}


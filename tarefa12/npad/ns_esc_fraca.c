// fraca_stdout.c
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>
#include <string.h>

#define NSTEPS 1000
#define DT 0.01f
#define VISCOSITY 0.1f

void run_simulation_fraca(int num_threads, int nx, int ny, const char *schedule_type, int chunk_size) {
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

    for (int i = 0; i < nx; i++)
        for (int j = 0; j < ny; j++)
            field[i][j] = 0.0f;
    field[nx / 2][ny / 2] = 1.0f;

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

    printf("%d,%d,%d,%s,%d,%.5f,%.5f,%.5f\n",
           num_threads, nx, ny, schedule_type, chunk_size,
           exec_time, center_value, avg_value);

    for (int i = 0; i < nx; i++) {
        free(field[i]);
        free(next_field[i]);
    }
    free(field);
    free(next_field);
}

int main() {
    printf("Threads,NX,NY,Schedule,ChunkSize,Time,CenterValue,AverageValue\n");

    int threads[] = {1, 2, 4, 8, 16};
    const char *schedules[] = {"static"};
    int chunk_sizes[] = {4};

    for (int t = 0; t < 5; t++) {
        int num_threads = threads[t];
        int nx = 100 * num_threads;
        int ny = 100 * num_threads;

        for (int s = 0; s < 1; s++) {
            for (int c = 0; c < 1; c++) {
                run_simulation_fraca(num_threads, nx, ny, schedules[s], chunk_sizes[c]);
            }
        }
    }

    return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

#define NX 100
#define NY 100
#define NSTEPS 1000
#define DT 0.01f
#define VISCOSITY 0.1f

void initialize_field(float field[NX][NY]) {
    for (int i = 0; i < NX; i++) {
        for (int j = 0; j < NY; j++) {
            field[i][j] = 0.0f;
        }
    }

    // Introduzindo uma pequena perturbação no centro
    field[NX/2][NY/2] = 1.0f;
}

void update_field(float current[NX][NY], float next[NX][NY]) {
    #pragma omp parallel for collapse(2) schedule(static)
    for (int i = 1; i < NX - 1; i++) {
        for (int j = 1; j < NY - 1; j++) {
            float laplacian = current[i+1][j] + current[i-1][j] + current[i][j+1] + current[i][j-1] - 4.0f * current[i][j];
            next[i][j] = current[i][j] + VISCOSITY * DT * laplacian;
        }
    }
}

void simulate(float field[NX][NY]) {
    float next_field[NX][NY];
    for (int step = 0; step < NSTEPS; step++) {
        update_field(field, next_field);

        #pragma omp parallel for collapse(2) schedule(static)
        for (int i = 0; i < NX; i++) {
            for (int j = 0; j < NY; j++) {
                field[i][j] = next_field[i][j];
            }
        }
    }
}

void run_simulation(int num_threads, const char *schedule_type, int chunk_size) {
    float field[NX][NY];
    initialize_field(field);

    omp_set_num_threads(num_threads);
    omp_set_schedule(omp_sched_static, chunk_size);

    double start_time = omp_get_wtime();
    simulate(field);
    double end_time = omp_get_wtime();

    double exec_time = end_time - start_time;

    FILE *file = fopen("results.csv", "a");
    fprintf(file, "%d,%s,%d,%.5f\n", num_threads, schedule_type, chunk_size, exec_time);
    fclose(file);
}

int main() {
    FILE *file = fopen("results.csv", "w");
    fprintf(file, "Threads,Schedule,ChunkSize,Time\n");
    fclose(file);

    int threads[] = {1, 2, 4, 8, 16};
    const char *schedules[] = {"static", "dynamic", "guided"};
    int chunk_sizes[] = {1, 4, 8};

    for (int t = 0; t < 5; t++) {
        for (int s = 0; s < 3; s++) {
            for (int c = 0; c < 3; c++) {
                run_simulation(threads[t], schedules[s], chunk_sizes[c]);
            }
        }
    }

    return 0;
}
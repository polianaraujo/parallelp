// forte_stdout.c
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>
#include <string.h>

#define NX 100
#define NY 100
#define NSTEPS 1000
#define DT 0.01f
#define VISCOSITY 0.1f

void initialize_field(float field[NX][NY]) {
    for (int i = 0; i < NX; i++)
        for (int j = 0; j < NY; j++)
            field[i][j] = 0.0f;

    field[NX / 2][NY / 2] = 1.0f;
}

void update_field(float current[NX][NY], float next[NX][NY]) {
    for (int i = 0; i < NX; i++) {
        next[i][0] = current[i][0];
        next[i][NY-1] = current[i][NY-1];
    }
    for (int j = 0; j < NY; j++) {
        next[0][j] = current[0][j];
        next[NX-1][j] = current[NX-1][j];
    }

    #pragma omp parallel for collapse(2) schedule(runtime)
    for (int i = 1; i < NX - 1; i++) {
        for (int j = 1; j < NY - 1; j++) {
            float laplacian = current[i+1][j] + current[i-1][j] +
                              current[i][j+1] + current[i][j-1] -
                              4.0f * current[i][j];
            next[i][j] = current[i][j] + VISCOSITY * DT * laplacian;
        }
    }
}

void simulate(float field[NX][NY]) {
    float next_field[NX][NY];

    for (int step = 0; step < NSTEPS; step++) {
        update_field(field, next_field);

        #pragma omp parallel for collapse(2) schedule(runtime)
        for (int i = 0; i < NX; i++)
            for (int j = 0; j < NY; j++)
                field[i][j] = next_field[i][j];
    }
}

void run_simulation(int num_threads, int chunk_size) {
    float field[NX][NY];
    initialize_field(field);

    omp_set_num_threads(num_threads);
    omp_set_schedule(omp_sched_static, chunk_size);

    double start_time = omp_get_wtime();
    simulate(field);
    double end_time = omp_get_wtime();

    double exec_time = end_time - start_time;
    float center_value = field[NX/2][NY/2];
    float sum = 0.0f;
    int count = 0;

    for (int i = 0; i < NX; i++)
        for (int j = 0; j < NY; j++)
            if (!isnan(field[i][j])) {
                sum += field[i][j];
                count++;
            }

    float avg_value = count > 0 ? sum / count : 0.0f;

    // Saída no terminal como CSV
    printf("%d,%d,%d,%s,%d,%.5f,%.5f,%.5f\n",
           num_threads, NX, NY, "static", chunk_size,
           exec_time, center_value, avg_value);
}

int main() {
    printf("Threads,NX,NY,Schedule,ChunkSize,Time,CenterValue,AverageValue\n");

    int threads[] = {1, 2, 4, 8, 16};
    int chunk_size = 8;

    for (int t = 0; t < 5; t++) {
        run_simulation(threads[t], chunk_size);
    }

    return 0;
}

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
    for (int i = 0; i < NX; i++) {
        for (int j = 0; j < NY; j++) {
            field[i][j] = 0.0f;
        }
    }

    // Pequena perturbação no centro
    // field[NX / 2][NY / 2] = 1.0f;
}

void update_field(float current[NX][NY], float next[NX][NY]) {
    // Copiar bordas diretamente (não paralelizado)
    for (int i = 0; i < NX; i++) {
        next[i][0] = current[i][0];
        next[i][NY-1] = current[i][NY-1];
    }
    for (int j = 0; j < NY; j++) {
        next[0][j] = current[0][j];
        next[NX-1][j] = current[NX-1][j];
    }

    // Atualizar interior com paralelismo
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

        // Trocar os campos
        #pragma omp parallel for collapse(2) schedule(runtime)
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

    if (strcmp(schedule_type, "static") == 0)
        omp_set_schedule(omp_sched_static, chunk_size);
    else if (strcmp(schedule_type, "dynamic") == 0)
        omp_set_schedule(omp_sched_dynamic, chunk_size);
    else if (strcmp(schedule_type, "guided") == 0)
        omp_set_schedule(omp_sched_guided, chunk_size);

    double start_time = omp_get_wtime();
    simulate(field);
    double end_time = omp_get_wtime();
    double exec_time = end_time - start_time;

    // Coletar valores de amostra da matriz final
    float center_value = field[NX/2][NY/2];
    float sum = 0.0f;
    int count = 0;
    for (int i = 0; i < NX; i++) {
        for (int j = 0; j < NY; j++) {
            if (!isnan(field[i][j])) {
                sum += field[i][j];
                count++;
            }
        }
    }
    float avg_value = count > 0 ? sum / count : 0.0f;

    // Salvar dados no CSV
    FILE *file = fopen("results_sem_perturbacao.csv", "a");
    if (file) {
        fprintf(file, "%d,%s,%d,%.5f,%.5f,%.5f\n",
            num_threads, schedule_type, chunk_size,
            exec_time, center_value, avg_value);
        fclose(file);
    }
}

int main() {
    FILE *file = fopen("results_sem_perturbacao.csv", "w");
    fprintf(file, "Threads,Schedule,ChunkSize,Time,CenterValue,AverageValue\n");
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

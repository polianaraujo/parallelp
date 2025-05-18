#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <omp.h>

#define NX 100
#define NY 100
#define NSTEPS 1000
#define DT 0.01f
#define VISCOSITY 0.1f

float field[NX][NY];
float next_field[NX][NY];

void initialize_field() {
    memset(field, 0, sizeof(field));
    field[NX / 2][NY / 2] = 1.0f;
}

void run_simulation_forte(int num_threads, const char *schedule_type, int chunk_size, const char *affinity_type) {
    omp_set_num_threads(num_threads);

    if (strcmp(schedule_type, "static") == 0)
        omp_set_schedule(omp_sched_static, chunk_size);
    else if (strcmp(schedule_type, "dynamic") == 0)
        omp_set_schedule(omp_sched_dynamic, chunk_size);
    else if (strcmp(schedule_type, "guided") == 0)
        omp_set_schedule(omp_sched_guided, chunk_size);

    initialize_field();

    double start_time = omp_get_wtime();

    for (int step = 0; step < NSTEPS; step++) {
        #pragma omp parallel for collapse(2) schedule(runtime)
        for (int i = 1; i < NX - 1; i++) {
            for (int j = 1; j < NY - 1; j++) {
                float laplacian = field[i+1][j] + field[i-1][j] +
                                  field[i][j+1] + field[i][j-1] -
                                  4.0f * field[i][j];
                next_field[i][j] = field[i][j] + VISCOSITY * DT * laplacian;
            }
        }

        // Copia o resultado para o campo atual
        memcpy(field, next_field, sizeof(field));
    }

    double end_time = omp_get_wtime();
    double exec_time = end_time - start_time;

    // Estatísticas simples
    float center_value = field[NX/2][NY/2];
    double sum = 0.0;
    for (int i = 0; i < NX; i++)
        for (int j = 0; j < NY; j++)
            sum += field[i][j];
    float avg_value = sum / (NX * NY);

    // Salvar resultados
    FILE *file = fopen("results_escalabilidade_forte.csv", "a");
    if (file) {
        fprintf(file, "%d,%d,%d,%s,%d,%s,%.5f,%.5f,%.5f\n",
                num_threads, NX, NY, schedule_type, chunk_size, affinity_type,
                exec_time, center_value, avg_value);
        fclose(file);
    }
}

int main(int argc, char *argv[]) {
    const char *affinity_type = (argc > 1) ? argv[1] : "undefined";

    FILE *file = fopen("results_escalabilidade_forte.csv", "r");
    int existe = (file != NULL);
    if (file) fclose(file);

    file = fopen("results_escalabilidade_forte.csv", "a");
    if (!file) {
        fprintf(stderr, "Erro ao abrir arquivo de resultados.\n");
        return 1;
    }
    if (!existe) {
        fprintf(file, "Threads,nx,ny,Schedule,ChunkSize,Affinity,ExecutionTime,CenterValue,AverageValue\n");
    }
    fclose(file);

    int threads[] = {1, 2, 4, 8, 16};
    const char *schedules[] = {"static"};
    int chunk_sizes[] = {4};

    for (int t = 0; t < 5; t++) {
        int num_threads = threads[t];
        for (int s = 0; s < 1; s++) {
            for (int c = 0; c < 1; c++) {
                run_simulation_forte(num_threads, schedules[s], chunk_sizes[c], affinity_type);
            }
        }
    }

    return 0;
}

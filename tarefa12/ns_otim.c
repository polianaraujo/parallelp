#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

#define NX 100
#define NY 100
#define NSTEPS 1000
#define DT 0.01f
#define VISCOSITY 0.1f

// Alocar matriz dinamicamente para melhor uso de cache
float** allocate_field() {
    float** field = malloc(NX * sizeof(float*));
    for (int i = 0; i < NX; i++)
        field[i] = malloc(NY * sizeof(float));
    return field;
}

void free_field(float** field) {
    for (int i = 0; i < NX; i++)
        free(field[i]);
    free(field);
}

void initialize_field(float** field) {
    #pragma omp parallel for collapse(2)
    for (int i = 0; i < NX; i++) {
        for (int j = 0; j < NY; j++) {
            field[i][j] = 0.0f;
        }
    }
    field[NX / 2][NY / 2] = 1.0f;
}

void update_field(float** current, float** next) {
    // Copiar bordas (agora paralelizado)
    #pragma omp parallel for
    for (int i = 0; i < NX; i++) {
        next[i][0] = current[i][0];
        next[i][NY-1] = current[i][NY-1];
    }

    #pragma omp parallel for
    for (int j = 0; j < NY; j++) {
        next[0][j] = current[0][j];
        next[NX-1][j] = current[NX-1][j];
    }

    // Atualização do interior
    #pragma omp parallel for collapse(2) schedule(static)
    for (int i = 1; i < NX - 1; i++) {
        for (int j = 1; j < NY - 1; j++) {
            float laplacian = current[i+1][j] + current[i-1][j] +
                              current[i][j+1] + current[i][j-1] -
                              4.0f * current[i][j];
            next[i][j] = current[i][j] + VISCOSITY * DT * laplacian;
        }
    }
}

void simulate(float** field) {
    float** next_field = allocate_field();

    for (int step = 0; step < NSTEPS; step++) {
        update_field(field, next_field);

        // Swap ponteiros (evita loop de cópia)
        float** temp = field;
        field = next_field;
        next_field = temp;
    }

    free_field(next_field);
}

void run_simulation(int num_threads, int chunk_size) {
    omp_set_num_threads(num_threads);
    omp_set_schedule(omp_sched_static, chunk_size);

    float** field = allocate_field();
    initialize_field(field);

    double start_time = omp_get_wtime();
    simulate(field);
    double end_time = omp_get_wtime();
    double exec_time = end_time - start_time;

    float center_value = field[NX/2][NY/2];
    float sum = 0.0f;

    #pragma omp parallel for reduction(+:sum) collapse(2)
    for (int i = 0; i < NX; i++) {
        for (int j = 0; j < NY; j++) {
            sum += field[i][j];
        }
    }
    float avg_value = sum / (NX * NY);

    FILE *file = fopen("results_forte_otimizada.csv", "a");
    if (file) {
        fprintf(file, "%d,%d,%d,%s,%d,%.5f,%.5f,%.5f\n",
            num_threads, NX, NY, "static", chunk_size,
            exec_time, center_value, avg_value);
        fclose(file);
    }

    free_field(field);
}

int main() {
    FILE *file = fopen("results_forte_otimizada.csv", "w");
    fprintf(file, "Threads,NX,NY,Schedule,ChunkSize,Time,CenterValue,AverageValue\n");
    fclose(file);

    int threads[] = {1, 2, 4, 8, 16};
    int chunk_size = 8;

    for (int t = 0; t < 5; t++) {
        run_simulation(threads[t], chunk_size);
    }

    return 0;
}

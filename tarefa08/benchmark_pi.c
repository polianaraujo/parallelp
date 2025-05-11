#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define N 100000000

double version_1A() {
    int total_hits = 0;

    double start = omp_get_wtime();
    #pragma omp parallel
    {
        int local_hits = 0;
        for (int i = 0; i < N / omp_get_num_threads(); i++) {
            float x = (float)rand() / RAND_MAX;
            float y = (float)rand() / RAND_MAX;
            if (x * x + y * y <= 1.0f) local_hits++;
        }
        #pragma omp critical
        total_hits += local_hits;
    }
    double end = omp_get_wtime();
    printf("1A (rand + critical): pi ≈ %.6f, tempo = %.3fs\n", 4.0 * total_hits / N, end - start);

    return end - start;
}

double version_2A() {
    int num_threads;
    #pragma omp parallel
    {
        #pragma omp single
        num_threads = omp_get_num_threads();
    }

    int* hits = calloc(num_threads, sizeof(int));
    double start = omp_get_wtime();

    #pragma omp parallel
    {
        int id = omp_get_thread_num();
        for (int i = 0; i < N / num_threads; i++) {
            float x = (float)rand() / RAND_MAX;
            float y = (float)rand() / RAND_MAX;
            if (x * x + y * y <= 1.0f) hits[id]++;
        }
    }

    int total_hits = 0;
    for (int i = 0; i < num_threads; i++) total_hits += hits[i];

    double end = omp_get_wtime();
    printf("2A (rand + vetor):     pi ≈ %.6f, tempo = %.3fs\n", 4.0 * total_hits / N, end - start);
    free(hits);

    return end - start;
}

double version_1B() {
    int total_hits = 0;

    double start = omp_get_wtime();
    #pragma omp parallel
    {
        int local_hits = 0;
        unsigned int seed = omp_get_thread_num();

        for (int i = 0; i < N / omp_get_num_threads(); i++) {
            float x = (float)rand_r(&seed) / RAND_MAX;
            float y = (float)rand_r(&seed) / RAND_MAX;
            if (x * x + y * y <= 1.0f) local_hits++;
        }

        #pragma omp critical
        total_hits += local_hits;
    }
    double end = omp_get_wtime();
    printf("1B (rand_r + critical): pi ≈ %.6f, tempo = %.3fs\n", 4.0 * total_hits / N, end - start);

    return end - start;
}

typedef struct {
    int value;
    char padding[60]; // evita múltiplos na mesma cache line (assumindo 64 bytes)
} PaddedInt;

double version_2B() {
    int num_threads;
    #pragma omp parallel
    {
        #pragma omp single
        num_threads = omp_get_num_threads();
    }

    // int* hits = calloc(num_threads, sizeof(int));
    
    PaddedInt* hits = calloc(num_threads, sizeof(PaddedInt));    

    double start = omp_get_wtime();

    #pragma omp parallel
    {
        int id = omp_get_thread_num();
        unsigned int seed = id;

        for (int i = 0; i < N / num_threads; i++) {
            float x = (float)rand_r(&seed) / RAND_MAX;
            float y = (float)rand_r(&seed) / RAND_MAX;
            // if (x * x + y * y <= 1.0f) hits[id]++;
            if (x * x + y * y <= 1.0f) hits[id].value++;
        }
    }

    int total_hits = 0;
    for (int i = 0; i < num_threads; i++){
        // total_hits += hits[i];
        total_hits += hits[i].value;
    }

    double end = omp_get_wtime();
    printf("2B (rand_r + vetor):    pi ≈ %.6f, tempo = %.3fs\n", 4.0 * total_hits / N, end - start);
    free(hits);

    return end - start;
}

void salvar_csv(double tempo_1A, double tempo_2A, double tempo_1B, double tempo_2B) {
    FILE *fp = fopen("tempos_pi.csv", "w");
    if (fp == NULL) {
        perror("Erro ao abrir o arquivo CSV");
        return;
    }
    fprintf(fp, "Versão,Tempo (s)\n");
    fprintf(fp, "1A: rand() + critical,%.6f\n", tempo_1A);
    fprintf(fp, "2A: rand() + vetor,%.6f\n", tempo_2A);
    fprintf(fp, "1B: rand_r() + critical,%.6f\n", tempo_1B);
    fprintf(fp, "2B: rand_r() + vetor padded,%.6f\n", tempo_2B);
    fclose(fp);
}


int main() {
    printf("Executando benchmark com %d pontos:\n\n", N);

    double t1 = version_1A();
    double t2 = version_2A();
    double t3 = version_1B();
    double t4 = version_2B();

    salvar_csv(t1, t2, t3, t4);

    return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h> // Necessário para gettimeofday()

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
    
    field[NX / 2][NY / 2] = 1.0f; // Perturbação inicial no centro
}

// Simula como o campo se comporta com o tempo
// Discretização explícita da equação de difusão
void update_field(float current[NX][NY], float next[NX][NY]) {
    for (int i = 1; i < NX - 1; i++) {
        for (int j = 1; j < NY - 1; j++) {
            float laplacian = current[i + 1][j] + current[i - 1][j] +
                              current[i][j + 1] + current[i][j - 1] -
                              4.0f * current[i][j];
            next[i][j] = current[i][j] + VISCOSITY * DT * laplacian;
        }
    }
}


void simulate(float field[NX][NY]) {
    float next_field[NX][NY];
    for (int step = 0; step < NSTEPS; step++) {
        update_field(field, next_field);

        // Atualiza o campo atual com o novo
        for (int i = 0; i < NX; i++) {
            for (int j = 0; j < NY; j++) {
                field[i][j] = next_field[i][j];
            }
        }
    }
}

void print_field(float field[NX][NY]) {
    for (int i = 0; i < NX; i += 10) {
        for (int j = 0; j < NY; j += 10) {
            printf("%.2f ", field[i][j]);
        }
        printf("\n");
    }
}

int main() {
    float field[NX][NY];
    initialize_field(field);

    struct timeval start, end;
    gettimeofday(&start, NULL); // Início da contagem

    simulate(field);

    gettimeofday(&end, NULL); // Fim da contagem

    // Cálculo do tempo em segundos
    double elapsed = (end.tv_sec - start.tv_sec) +
                     (end.tv_usec - start.tv_usec) / 1e6;

    printf("Tempo de execução da simulação: %.6f segundos\n\n", elapsed);

    print_field(field);

    return 0;
}

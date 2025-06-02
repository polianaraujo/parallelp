#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>

// Função para alocar matriz 3D
float ***alloc_3d(int N) {
    float ***array = malloc(N * sizeof(float **));
    if (array == NULL) return NULL; // Verifica falha na alocação
    for (int i = 0; i < N; i++) {
        array[i] = malloc(N * sizeof(float *));
        if (array[i] == NULL) { // Verifica falha e libera o que já foi alocado
            for(int k=0; k<i; ++k) free(array[k]);
            free(array);
            return NULL;
        }
        for (int j = 0; j < N; j++) {
            array[i][j] = calloc(N, sizeof(float));
            if (array[i][j] == NULL) { // Verifica falha e libera
                for(int k=0; k<j; ++k) free(array[i][k]);
                for(int k=0; k<i; ++k) { // Libera todas as linhas anteriores
                    for(int l=0; l<N; ++l) free(array[k][l]); // Supondo N colunas alocadas
                    free(array[k]);
                }
                free(array[i]); // Libera a linha atual (parcialmente alocada)
                free(array);
                return NULL;
            }
        }
    }
    return array;
}

// Função para liberar matriz 3D
void free_3d(float ***array, int N) {
    if (array == NULL) return;
    for (int i = 0; i < N; i++) {
        if (array[i] == NULL) continue;
        for (int j = 0; j < N; j++) {
            if (array[i][j] != NULL) {
                free(array[i][j]);
            }
        }
        free(array[i]);
    }
    free(array);
}

// Função para inicializar a matriz
void initialize(float ***u, int N) {
    if (N <= 0) return; // Evita divisão por zero ou acesso inválido
    int cx = N / 2, cy = N / 2, cz = N / 2;
    // Garante que cx, cy, cz estejam dentro dos limites, especialmente para N pequeno
    if (cx >= 0 && cx < N && cy >= 0 && cy < N && cz >= 0 && cz < N) {
       if (u && u[cx] && u[cx][cy]) { // Verifica se os ponteiros são válidos
            u[cx][cy][cz] = 1.0f;
       }
    }
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("Uso: %s <schedule: static|dynamic|guided> <collapse: 1|2|3> <N_base>\n", argv[0]);
        return 1;
    }

    char *schedule_arg = argv[1];
    int collapse_level = atoi(argv[2]);
    int N_base = atoi(argv[3]);

    // Validação inicial dos parâmetros
    if (N_base <= 0) {
        printf("N_base deve ser um inteiro positivo.\n");
        return 1;
    }
    if (collapse_level < 1 || collapse_level > 3) {
        printf("Collapse inválido. Use 1, 2 ou 3.\n");
        return 1;
    }
    if (strcmp(schedule_arg, "static") != 0 && strcmp(schedule_arg, "dynamic") != 0 && strcmp(schedule_arg, "guided") != 0) {
        printf("Agendamento inválido. Use static, dynamic ou guided.\n");
        return 1;
    }

    int n_threads = 1;
    #pragma omp parallel
    {
        #pragma omp master
        n_threads = omp_get_num_threads();
    }

    int N = N_base * n_threads;
    if (N < 3 && N > 0) { // Stencil requer pelo menos 3x3x3 para ter pontos internos
        printf("N (%d) é muito pequeno para o stencil (N_base * n_threads). Deve ser >= 3.\n", N);
        // Se N=0 (N_base=0 ou n_threads=0), alloc_3d pode falhar ou N/2 em initialize ser problemático
        // N_base já foi validado como >0, n_threads é >=1.
        // O problema é que i, j, k vão de 1 a N-1. Se N=1 ou N=2, N-1 é 0 ou 1. Loop for(i=1; i<0) não executa.
        // A lógica do stencil i+1, i-1 etc., assume que 0 e N-1 são bordas e não são calculados.
        // Portanto, N precisa ser pelo menos 3 para que N-1 seja 2, e o loop for(i=1; i<2; i++) execute para i=1.
        return 1;
    }
     if (N <= 0) { // Segurança adicional se N_base*n_threads resultar em 0 ou negativo (improvável com validações)
        printf("N (%d) inválido.\n", N);
        return 1;
    }


    int NSTEPS = 100;
    float DT = 0.01f, DX = 1.0f, VISC = 0.1f;

    float ***u = alloc_3d(N);
    float ***u_new = alloc_3d(N);

    if (u == NULL || u_new == NULL) {
        printf("Falha na alocação de memória para u ou u_new.\n");
        free_3d(u, N); // Tenta liberar o que pode ter sido alocado
        free_3d(u_new, N);
        return 1;
    }

    initialize(u, N);

    double start = omp_get_wtime();

    for (int step = 0; step < NSTEPS; step++) {
        if (strcmp(schedule_arg, "static") == 0) {
            if (collapse_level == 1) {
                #pragma omp parallel for schedule(static) collapse(1)
                for (int i = 1; i < N - 1; i++) {
                    for (int j = 1; j < N - 1; j++) {
                        for (int k = 1; k < N - 1; k++) {
                            float laplacian = (u[i + 1][j][k] + u[i - 1][j][k] +
                                               u[i][j + 1][k] + u[i][j - 1][k] +
                                               u[i][j][k + 1] + u[i][j][k - 1] -
                                               6.0f * u[i][j][k]) / (DX * DX);
                            u_new[i][j][k] = u[i][j][k] + DT * VISC * laplacian;
                        }
                    }
                }
            } else if (collapse_level == 2) {
                #pragma omp parallel for schedule(static) collapse(2)
                for (int i = 1; i < N - 1; i++) {
                    for (int j = 1; j < N - 1; j++) {
                        for (int k = 1; k < N - 1; k++) {
                            float laplacian = (u[i + 1][j][k] + u[i - 1][j][k] +
                                               u[i][j + 1][k] + u[i][j - 1][k] +
                                               u[i][j][k + 1] + u[i][j][k - 1] -
                                               6.0f * u[i][j][k]) / (DX * DX);
                            u_new[i][j][k] = u[i][j][k] + DT * VISC * laplacian;
                        }
                    }
                }
            } else { // collapse_level == 3
                #pragma omp parallel for schedule(static) collapse(3)
                for (int i = 1; i < N - 1; i++) {
                    for (int j = 1; j < N - 1; j++) {
                        for (int k = 1; k < N - 1; k++) {
                            float laplacian = (u[i + 1][j][k] + u[i - 1][j][k] +
                                               u[i][j + 1][k] + u[i][j - 1][k] +
                                               u[i][j][k + 1] + u[i][j][k - 1] -
                                               6.0f * u[i][j][k]) / (DX * DX);
                            u_new[i][j][k] = u[i][j][k] + DT * VISC * laplacian;
                        }
                    }
                }
            }
        } else if (strcmp(schedule_arg, "dynamic") == 0) {
            if (collapse_level == 1) {
                #pragma omp parallel for schedule(dynamic) collapse(1)
                for (int i = 1; i < N - 1; i++) {
                    for (int j = 1; j < N - 1; j++) {
                        for (int k = 1; k < N - 1; k++) {
                            float laplacian = (u[i + 1][j][k] + u[i - 1][j][k] +
                                               u[i][j + 1][k] + u[i][j - 1][k] +
                                               u[i][j][k + 1] + u[i][j][k - 1] -
                                               6.0f * u[i][j][k]) / (DX * DX);
                            u_new[i][j][k] = u[i][j][k] + DT * VISC * laplacian;
                        }
                    }
                }
            } else if (collapse_level == 2) {
                #pragma omp parallel for schedule(dynamic) collapse(2)
                for (int i = 1; i < N - 1; i++) {
                    for (int j = 1; j < N - 1; j++) {
                        for (int k = 1; k < N - 1; k++) {
                            float laplacian = (u[i + 1][j][k] + u[i - 1][j][k] +
                                               u[i][j + 1][k] + u[i][j - 1][k] +
                                               u[i][j][k + 1] + u[i][j][k - 1] -
                                               6.0f * u[i][j][k]) / (DX * DX);
                            u_new[i][j][k] = u[i][j][k] + DT * VISC * laplacian;
                        }
                    }
                }
            } else { // collapse_level == 3
                #pragma omp parallel for schedule(dynamic) collapse(3)
                for (int i = 1; i < N - 1; i++) {
                    for (int j = 1; j < N - 1; j++) {
                        for (int k = 1; k < N - 1; k++) {
                            float laplacian = (u[i + 1][j][k] + u[i - 1][j][k] +
                                               u[i][j + 1][k] + u[i][j - 1][k] +
                                               u[i][j][k + 1] + u[i][j][k - 1] -
                                               6.0f * u[i][j][k]) / (DX * DX);
                            u_new[i][j][k] = u[i][j][k] + DT * VISC * laplacian;
                        }
                    }
                }
            }
        } else if (strcmp(schedule_arg, "guided") == 0) {
            if (collapse_level == 1) {
                #pragma omp parallel for schedule(guided) collapse(1)
                for (int i = 1; i < N - 1; i++) {
                    for (int j = 1; j < N - 1; j++) {
                        for (int k = 1; k < N - 1; k++) {
                            float laplacian = (u[i + 1][j][k] + u[i - 1][j][k] +
                                               u[i][j + 1][k] + u[i][j - 1][k] +
                                               u[i][j][k + 1] + u[i][j][k - 1] -
                                               6.0f * u[i][j][k]) / (DX * DX);
                            u_new[i][j][k] = u[i][j][k] + DT * VISC * laplacian;
                        }
                    }
                }
            } else if (collapse_level == 2) {
                #pragma omp parallel for schedule(guided) collapse(2)
                for (int i = 1; i < N - 1; i++) {
                    for (int j = 1; j < N - 1; j++) {
                        for (int k = 1; k < N - 1; k++) {
                            float laplacian = (u[i + 1][j][k] + u[i - 1][j][k] +
                                               u[i][j + 1][k] + u[i][j - 1][k] +
                                               u[i][j][k + 1] + u[i][j][k - 1] -
                                               6.0f * u[i][j][k]) / (DX * DX);
                            u_new[i][j][k] = u[i][j][k] + DT * VISC * laplacian;
                        }
                    }
                }
            } else { // collapse_level == 3
                #pragma omp parallel for schedule(guided) collapse(3)
                for (int i = 1; i < N - 1; i++) {
                    for (int j = 1; j < N - 1; j++) {
                        for (int k = 1; k < N - 1; k++) {
                            float laplacian = (u[i + 1][j][k] + u[i - 1][j][k] +
                                               u[i][j + 1][k] + u[i][j - 1][k] +
                                               u[i][j][k + 1] + u[i][j][k - 1] -
                                               6.0f * u[i][j][k]) / (DX * DX);
                            u_new[i][j][k] = u[i][j][k] + DT * VISC * laplacian;
                        }
                    }
                }
            }
        }

        // Troca os ponteiros
        float ***temp = u;
        u = u_new;
        u_new = temp;
    }

    double end = omp_get_wtime();
    double elapsed = end - start;

    printf("%d,%s,%d,%d,%.6f\n", n_threads, schedule_arg, collapse_level, N, elapsed);

    free_3d(u, N);
    free_3d(u_new, N);
    return 0;
}
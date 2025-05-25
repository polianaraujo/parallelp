#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <omp.h>

double *malloc_and_init(int n, double value) {
    double *array = malloc(n * n * sizeof(double));
    for (int i = 0; i < n * n; ++i) {
        array[i] = value;
    }
    return array;
}

int main(int argc, char **argv) {
    if (argc < 3) {
        printf("Uso: %s <tamanho da matriz> <número de passos>\n", argv[0]);
        return 1;
    }

    int n = atoi(argv[1]);
    int nsteps = atoi(argv[2]);

    double alpha = 0.25;
    double dx = 1.0 / n;
    double dt = dx * dx / 4.0;

    double *u = malloc_and_init(n, 0.0);
    double *u_tmp = malloc_and_init(n, 0.0);

    // Condição inicial (exemplo simples: bordas quentes)
    for (int i = 0; i < n; ++i) {
        u[i] = 100.0;
        u[(n-1)*n + i] = 100.0;
        u[i*n] = 100.0;
        u[i*n + n-1] = 100.0;
    }

    // Enviar dados para a GPU
    #pragma omp target enter data map(to: u[0:n*n], u_tmp[0:n*n])

    for (int t = 0; t < nsteps; ++t) {
        #pragma omp target teams distribute parallel for collapse(2)
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                const double r = alpha * dt / (dx * dx);
                const double r2 = 1.0 - 4.0 * r;

                u_tmp[i + j * n] = r2 * u[i + j * n] +
                    r * ((i < n - 1) ? u[(i + 1) + j * n] : 0.0) +
                    r * ((i > 0)     ? u[(i - 1) + j * n] : 0.0) +
                    r * ((j < n - 1) ? u[i + (j + 1) * n] : 0.0) +
                    r * ((j > 0)     ? u[i + (j - 1) * n] : 0.0);
            }
        }

        // Troca dos ponteiros na CPU
        double *tmp = u;
        u = u_tmp;
        u_tmp = tmp;
    }

    // Trazer dados de volta da GPU para a CPU
    #pragma omp target update from(u[0:n*n])

    // Cálculo do erro L2
    double l2norm = 0.0;
    for (int i = 0; i < n * n; ++i) {
        l2norm += u[i] * u[i];
    }
    l2norm = sqrt(l2norm);

    printf("L2-norm: %f\n", l2norm);

    // Liberar dados da GPU
    #pragma omp target exit data map(delete: u[0:n*n], u_tmp[0:n*n])

    // Liberar memória na CPU
    free(u);
    free(u_tmp);

    return 0;
}

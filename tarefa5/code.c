#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <math.h>

// Função que verifica se um número é primo
int eh_primo(int num) {
    if (num < 2) return 0;
    for (int i = 2; i <= sqrt(num); i++) {
        if (num % i == 0)
            return 0;
    }
    return 1;
}

int main() {
    int n, i, total_primos_seq = 0, total_primos_paralelo = 0;

    printf("Digite o valor de n: ");
    scanf("%d", &n);

    double start, end;

    // ================= SEQUENCIAL =================
    start = omp_get_wtime();
    for (i = 2; i <= n; i++) {
        if (eh_primo(i)) {
            total_primos_seq++;
        }
    }
    end = omp_get_wtime();
    printf("\n[Sequencial] Total de primos entre 2 e %d: %d\n", n, total_primos_seq);
    printf("[Sequencial] Tempo: %f segundos\n", end - start);

    // ================= PARALELO =================
    start = omp_get_wtime();
    int nthreads = 0;

    #pragma omp parallel
    {
        #pragma omp single
        {
            nthreads = omp_get_num_threads();
        }

        #pragma omp for reduction(+:total_primos_paralelo)
        for (i = 2; i <= n; i++) {
            if (eh_primo(i)) {
                total_primos_paralelo++;
            }
        }
    }
    end = omp_get_wtime();

    printf("\n[Paralelo] Total de primos entre 2 e %d: %d\n", n, total_primos_paralelo);
    printf("[Paralelo] Tempo: %f segundos\n", end - start);
    printf("[Paralelo] Threads: %d\n", nthreads);  // <-- Adiciona esta linha


    return 0;
}

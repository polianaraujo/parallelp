#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

// Função para checar se um número é primo
int eh_primo(int num) {
    if (num < 2) return 0;
    for (int i = 2; i*i <= num; i++) {
        if (num % i == 0) return 0;
    }
    return 1;
}

int main() {
    int n;
    printf("Digite o valor máximo (n): ");
    scanf("%d", &n);

    int count_seq = 0;
    int count_paralelo = 0;

    double start_seq, end_seq;
    double start_par, end_par;

    // Versão sequencial
    start_seq = omp_get_wtime();
    for (int i = 2; i <= n; i++) {
        if (eh_primo(i)) {
            count_seq++;
        }
    }
    end_seq = omp_get_wtime();

    printf("\nSequencial:\n");
    printf("Total de primos entre 2 e %d: %d\n", n, count_seq);
    printf("Tempo sequencial: %.6f segundos\n", end_seq - start_seq);

    // Versão paralela
    start_par = omp_get_wtime();
    #pragma omp parallel for reduction(+:count_paralelo)
    for (int i = 2; i <= n; i++) {
        if (eh_primo(i)) {
            count_paralelo++;
        }
    }
    end_par = omp_get_wtime();

    printf("\nParalelo:\n");
    printf("Total de primos entre 2 e %d: %d\n", n, count_paralelo);
    printf("Tempo paralelo: %.6f segundos\n", end_par - start_par);

    // Verificando se o resultado bate
    if (count_seq == count_paralelo) {
        printf("\n✅ Resultados coincidem.\n");
    } else {
        printf("\n❌ Resultados diferentes!\n");
    }

    return 0;
}

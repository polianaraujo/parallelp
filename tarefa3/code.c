/*
Implemente um programa em C que calcule uma aproximação de ∏ usando uma série matemática, variando o número de iterações e medindo o tempo de execução. Compare os valores obtidos com o valor real de π e analise como a acurácia melhora com mais processamento. Reflita sobre como esse comportamento se repete em aplicações reais que demandam resultados cada vez mais precisos, como simulações físicas e inteligência artificial.
*/

#include <stdio.h>
#include <math.h>
#include <time.h>

// Função para calcular pi pela série de Leibniz
double calcular_pi(long long int iteracoes) {
    double pi = 0.0;
    for (long long int k = 0; k < iteracoes; k++) {
        double termo = (k % 2 == 0 ? 1.0 : -1.0) / (2.0 * k + 1.0);
        pi += termo;
    }
    return 4.0 * pi;
}

int main() {
    // Vetor com diferentes quantidades de iterações
    long long int passos[] = {1000, 10000, 100000, 1000000, 10000000};
    int tamanho = sizeof(passos) / sizeof(passos[0]);
    double pi_real = M_PI;

    printf("--------------------------------------------------------------------------\n");
    printf("Iterações\tπ calculado\t\tErro Absoluto\t\tTempo (s)\n");
    printf("--------------------------------------------------------------------------\n");

    for (int i = 0; i < tamanho; i++) {
        clock_t inicio = clock();  // Tempo inicial
        double pi_aprox = calcular_pi(passos[i]);
        clock_t fim = clock();     // Tempo final
        double tempo_exec = (double)(fim - inicio) / CLOCKS_PER_SEC;

        double erro = fabs(pi_real - pi_aprox);  // Erro absoluto entre os dois valores.

        printf("%lld\t\t%.15f\t%.15f\t%.5f\n", passos[i], pi_aprox, erro, tempo_exec);
    }

    return 0;
}

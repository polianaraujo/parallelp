/*
Implemente um programa em C que calcule uma aproximação de ∏ usando uma série matemática, variando o número de iterações e medindo o tempo de execução. Compare os valores obtidos com o valor real de π e analise como a acurácia melhora com mais processamento. Reflita sobre como esse comportamento se repete em aplicações reais que demandam resultados cada vez mais precisos, como simulações físicas e inteligência artificial.
*/

#include <stdio.h>
#include <math.h>
#include <sys/time.h>

// Função para calcular pi usando a série de Leibniz
double calcular_pi(long long int iteracoes) {
    double pi = 0.0;
    for (long long int k = 0; k < iteracoes; k++) {
        double termo = (k % 2 == 0 ? 1.0 : -1.0) / (2.0 * k + 1.0);
        pi += termo;
    }
    return 4.0 * pi;
}

// Função para calcular tempo decorrido em segundos
double tempo_decorrido(struct timeval inicio, struct timeval fim) {
    return (fim.tv_sec - inicio.tv_sec) + (fim.tv_usec - inicio.tv_usec) / 1e6;
}

int main() {
    long long int passos[] = {1000, 10000, 100000, 1000000, 10000000};
    int tamanho = sizeof(passos) / sizeof(passos[0]);
    double pi_real = M_PI;

    printf("--------------------------------------------------------------------------\n");
    printf("Iterações\tπ calculado\t\tErro Absoluto\t\tTempo (s)\n");
    printf("--------------------------------------------------------------------------\n");

    for (int i = 0; i < tamanho; i++) {
        struct timeval inicio, fim;

        gettimeofday(&inicio, NULL);  // Captura o tempo antes do cálculo
        double pi_aprox = calcular_pi(passos[i]);
        gettimeofday(&fim, NULL);  // Captura o tempo depois do cálculo

        double tempo_exec = tempo_decorrido(inicio, fim);
        double erro = fabs(pi_real - pi_aprox);

        printf("%lld\t\t%.15f\t%.15f\t%.6f\n", passos[i], pi_aprox, erro, tempo_exec);
    }

    return 0;
}

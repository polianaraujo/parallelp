#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h> // Para gettimeofday()

// Função para calcular a aproximação de Pi usando a série de Leibniz
double calcular_pi(long long int iteracoes) {
    double pi_aproximado = 0.0;
    int sinal = 1;
    for (long long int i = 0; i < iteracoes; i++) {
        pi_aproximado += sinal * (4.0 / (2.0 * i + 1.0));
        sinal *= -1;
    }
    return pi_aproximado;
}

int main() {
    // Array com diferentes números de iterações para testar
    long long int iteracoes[] = {100, 1000, 10000, 100000, 1000000, 10000000, 100000000, 1000000000};
    int num_niveis = sizeof(iteracoes) / sizeof(iteracoes[0]);

    // Ponteiro para o arquivo
    FILE *arquivo_csv;

    // Abre (ou cria) o arquivo "resultados_pi.csv" em modo de escrita ("w")
    arquivo_csv = fopen("resultados_pi.csv", "w");

    // Verifica se o arquivo foi aberto com sucesso
    if (arquivo_csv == NULL) {
        printf("Erro ao abrir o arquivo CSV!\n");
        return 1; // Retorna um código de erro
    }

    printf("Calculando aproximações de Pi e gerando arquivo CSV...\n");

    // Escreve o cabeçalho no arquivo CSV
    fprintf(arquivo_csv, "Iteracoes,Pi_Calculado,Diferenca,Tempo_s\n");

    for (int i = 0; i < num_niveis; i++) {
        struct timeval inicio, fim;

        // Marca o tempo de início
        gettimeofday(&inicio, NULL);

        double pi_calculado = calcular_pi(iteracoes[i]);

        // Marca o tempo de fim
        gettimeofday(&fim, NULL);

        // Calcula o tempo gasto em segundos
        double tempo_gasto = (fim.tv_sec - inicio.tv_sec) + (fim.tv_usec - inicio.tv_usec) / 1000000.0;
        
        double diferenca = fabs(M_PI - pi_calculado);

        // Imprime os resultados no console para acompanhamento
        printf("Iterações: %-15lld | Tempo: %-10.6f s\n", iteracoes[i], tempo_gasto);

        // Escreve uma linha de dados no arquivo CSV
        fprintf(arquivo_csv, "%lld,%.15f,%.15f,%.6f\n", iteracoes[i], pi_calculado, diferenca, tempo_gasto);
    }

    // Fecha o arquivo para salvar as alterações
    fclose(arquivo_csv);

    printf("\nArquivo 'resultados_pi.csv' gerado com sucesso!\n");

    return 0;
}
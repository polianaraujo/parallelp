/*
    # pragma omp atomic
    clausula reduction

    test_and_set
    swap_and_compare
*/

// Implemente novamente o estimador da tarefa 8 que usa contadores privados e o rand_r sibstituindo o #pragma omp critical pelo #pragma omp atomic. Compare as duas implementações com uma 3ª versão que utiliza apenas a cláusula #reduction ao invés das diretivas de sincronização. Reflita sobre a aplicabilidade desses mecanismos em termos de desempenho e produtividade e proponha um roteiro para quando utilizar qual mecanismo de sincronização.


// Aqui, a ideia é substituir a seção crítica por uma operação atômica, que garante que a variável global seja atualizada de forma segura sem a necessidade de bloqueio explícito.

#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

int main() {
    int i, count = 0, total_points = 100000000;
    double x, y;
    double start = omp_get_wtime();

    #pragma omp parallel
    {
        int local_count = 0;
        unsigned int seed = omp_get_thread_num();
        #pragma omp for
        for (i = 0; i < total_points; i++) {
            x = (double)rand_r(&seed) / RAND_MAX;
            y = (double)rand_r(&seed) / RAND_MAX;
            if (x*x + y*y <= 1.0)
                local_count++;
        }
        #pragma omp atomic
        count += local_count;
    }

    double pi = 4.0 * count / total_points;
    double end = omp_get_wtime();

    printf("Versão 1 - rand_r() + atomic\nπ ≈ %.10f\nTempo: %.4fs\n", pi, end - start);
    return 0;
}

// Explicação da versão 1:
// A operação #pragma omp atomic permite que a atualização da variável count seja feita de forma atômica, evitando condições de corrida sem usar um bloqueio explícito.

// Em vez de aguardar que todas as threads terminem sua contagem (como no #pragma omp critical), cada thread atualiza a variável global individualmente, mas de forma controlada, sem conflitos de concorrência.
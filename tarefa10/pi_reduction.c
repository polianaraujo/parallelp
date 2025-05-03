// rand_r() com #pragma omp reduction
// Nesta versão, utilizamos a cláusula reduction do OpenMP, que permite a redução dos resultados de cada thread em um valor final sem a necessidade de sincronização explícita. A variável count será somada localmente em cada thread e, ao final, essas somas serão combinadas em uma operação de redução.

#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

int main() {
    int i, total_points = 100000000;
    double x, y;
    int count = 0;
    double start = omp_get_wtime();

    #pragma omp parallel for reduction(+:count)
    for (i = 0; i < total_points; i++) {
        unsigned int seed = omp_get_thread_num();
        x = (double)rand_r(&seed) / RAND_MAX;
        y = (double)rand_r(&seed) / RAND_MAX;
        if (x*x + y*y <= 1.0)
            count++;
    }

    double pi = 4.0 * count / total_points;
    double end = omp_get_wtime();

    printf("Versão 2 - rand_r() + reduction\nπ ≈ %.10f\nTempo: %.4fs\n", pi, end - start);
    return 0;
}

// Explicação da versão 2:
// A cláusula reduction cria uma cópia privada da variável count para cada thread. Ao final, as cópias privadas são combinadas (reduzidas) automaticamente, sem a necessidade de manipulação explícita da variável global.

// A cláusula reduction(+:count) especifica que as cópias privadas de count devem ser somadas de forma segura, gerando o valor final.
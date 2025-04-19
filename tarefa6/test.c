/*
 * Estimativa estocástica de pi - versões serial, paralela incorreta e paralela correta com OpenMP
 */

 #include <stdio.h>
 #include <stdlib.h>
 #include <time.h>
 #include <omp.h>
 
 int main() {
     long int N = 1000000;
     long int dentro_do_circulo = 0;
     double x, y;
 
     printf("Versão Serial:\n");
     srand(time(NULL));
     for (long int i = 0; i < N; i++) {
         x = (double)rand() / RAND_MAX;
         y = (double)rand() / RAND_MAX;
         if (x*x + y*y <= 1.0) {
             dentro_do_circulo++;
         }
     }
     double pi = 4.0 * dentro_do_circulo / N;
     printf("Pi = %f\n\n", pi);
 
     // Resetando
     dentro_do_circulo = 0;
 
     printf("Versão Paralela Incorreta:\n");
     srand(time(NULL));
     #pragma omp parallel for
     for (long int i = 0; i < N; i++) {
         double x = (double)rand() / RAND_MAX;
         double y = (double)rand() / RAND_MAX;
         if (x*x + y*y <= 1.0) {
             dentro_do_circulo++; // Condição de corrida
         }
     }
     pi = 4.0 * dentro_do_circulo / N;
     printf("Pi (incorreto) = %f\n\n", pi);
 
     // Resetando
     dentro_do_circulo = 0;
 
     printf("Versão Paralela Correta:\n");
     #pragma omp parallel default(none) shared(N, dentro_do_circulo) private(x, y){
        unsigned int seed = time(NULL) ^ omp_get_thread_num();
        double x, y;

        // Laço paralelo com reduction (para somar sem corrida)
        #pragma omp for reduction(+:dentro_do_circulo)
        for (long i = 0; i < N; i++) {
            x = (double)rand_r(&seed) / RAND_MAX;
            y = (double)rand_r(&seed) / RAND_MAX;

            if (x * x + y * y <= 1.0) {
                dentro_do_circulo++;
            }
        }
    }

    pi = 4.0 * (double)dentro_do_circulo / (double)N;

    printf("Estimativa de pi: %f\n", pi);
    return 0;
}
 


//  Erro da versão paralela incorreta:
// O erro ocorre porque várias threads atualizam dentro_do_circulo++ simultaneamente, gerando uma condição de corrida.

// Correção:

// Uso de #pragma omp parallel para abrir a região paralela.

// #pragma omp for reduction(+:dentro_do_circulo) para acumular corretamente.

// private(x, y) para garantir que cada thread tenha suas próprias variáveis.

// default(none) obriga a declarar explicitamente o escopo das variáveis, aumentando a segurança e clareza.
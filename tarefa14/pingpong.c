#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MIN_SIZE 8           // 8 bytes
#define MAX_SIZE (1024*1024) // 1 MB
#define NUM_ITERATIONS 1000  // Numero de trocas

int main(int argc, char** argv) {

    int rank, size;
    MPI_Init(&argc, &argv);                 // Inicialização do MPI
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);   // ID do processo atual
    MPI_Comm_size(MPI_COMM_WORLD, &size);   // Quantos processos existem no total

    // Interrompe a execução se não forem exatamente 2 processos.
    if (size != 2) {
        if (rank == 0) {
            fprintf(stderr, "Este programa precisa ser executado com exatamente 2 processos.\n");
        }
        MPI_Finalize(); // Finaliza o ambiente MPI
        return 1;
    }

    FILE *output = NULL;
    if (rank == 0) {    // Processo 0 abre o csv e escreve o cabeçalho
        output = fopen("tempo_pingpong.csv", "w");
        fprintf(output, "Tamanho(Bytes),Tempo_Total(s),Tempo_Medio(s)\n");
    }

    // Iteração sobre os diferentes tamanhos de mensagem
    for (int msg_size = MIN_SIZE; msg_size <= MAX_SIZE; msg_size *= 2) {

        // Aloca e inicializa o buffer de mensagem
        char *buffer = (char*) malloc(msg_size);
        memset(buffer, 'a', msg_size);

        MPI_Barrier(MPI_COMM_WORLD); // sincroniza antes de começar a medir

        double start_time = MPI_Wtime();

        for (int i = 0; i < NUM_ITERATIONS; i++) {  // Iteração de ida e volta
            if (rank == 0) {    // Processo 0: envia e logo recebe a resposta
                MPI_Send(buffer, msg_size, MPI_CHAR, 1, 0, MPI_COMM_WORLD);
                MPI_Recv(buffer, msg_size, MPI_CHAR, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            } else if (rank == 1) {    // Processo 1: recebe e envia de volta
                MPI_Recv(buffer, msg_size, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                MPI_Send(buffer, msg_size, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
            }
        }

        double end_time = MPI_Wtime();
        double total_time = end_time - start_time;
        double avg_time = total_time / (NUM_ITERATIONS * 2); // cada iteração tem 2 mensagens
        
        // Processo 0 escreve os dados no csv e imprime no terminal
        if (rank == 0) {
            fprintf(output, "%d,%lf,%lf\n", msg_size, total_time, avg_time);
            printf("Tamanho: %d bytes, Tempo total: %.6lf s, Tempo médio: %.9lf s\n", msg_size, total_time, avg_time);
        }

        free(buffer);   // Libera memória
    }

    // Fecha o arquivo
    if (rank == 0) {
        fclose(output);
        printf("\nResultados salvos em tempo_pingpong.csv\n");
    }

    MPI_Finalize();
    return 0;
}

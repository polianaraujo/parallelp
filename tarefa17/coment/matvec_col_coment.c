#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main(int argc, char *argv[]) {
    int rank, size, M, N; // rank: ID do processo; size: número total de processos; M, N: dimensões da matriz

    // Inicializa o ambiente MPI. Esta é a primeira chamada MPI em qualquer programa MPI.
    MPI_Init(&argc, &argv);

    // Obtém o rank (identificador único) do processo atual dentro do comunicador MPI_COMM_WORLD.
    // O rank 0 é geralmente o processo mestre.
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // Obtém o número total de processos no comunicador MPI_COMM_WORLD.
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Validação de argumentos e divisibilidade

    // Verifica se o número de argumentos está correto (nome do programa + M + N)
    if (argc != 3) {
        if (rank == 0)
            printf("Uso: %s <M> <N>\n", argv[0]);
        MPI_Finalize();
        return -1;
    }

    M = atoi(argv[1]);  // Converte o primeiro argumento (M) para inteiro
    N = atoi(argv[2]);  // Converte o segundo argumento (N) para inteiro

    // Garante que o número de colunas (N) é divisível pelo número de processos (size), para distribuição uniforme de colunas
    if (N % size != 0) {
        if (rank == 0)  // // Apenas o processo mestre imprime a mensagem de uso
            printf("Erro: N (%d) deve ser divisível pelo número de processos (%d).\n", N, size);
        MPI_Finalize();
        return -1;
    }

    // Número de colunas que cada processo irá manipular
    int local_cols = N / size;

    // Alocação de memória
    double *A = NULL;    // Ponteiro
    double *x = NULL;    // Ponteiro

    // local_A: Armazena o bloco de colunas da matriz A que este processo irá receber.
    // É alocado para todos os processos. Tamanho: M linhas * local_cols colunas.
    double *local_A = (double *)malloc(M * local_cols * sizeof(double));

    // local_x: Armazena o segmento do vetor x que este processo irá receber.
    // É alocado para todos os processos. Tamanho: local_cols elementos.
    double *local_x = (double *)malloc(local_cols * sizeof(double));

    // local_y: Armazena a contribuição parcial deste processo para o vetor resultado y.
    // É alocado para todos os processos e inicializado com zeros (calloc). Tamanho: M elementos.
    double *local_y = (double *)calloc(M, sizeof(double));
    double *y = NULL;   // Ponteiro
    
    // Apenas o processo mestre (rank 0) aloca e inicializa as matrizes e vetores completos
    if (rank == 0) {
        A = (double *)malloc(M * N * sizeof(double));
        x = (double *)malloc(N * sizeof(double));
        y = (double *)malloc(M * sizeof(double));
        
        // Inicializa todos os elementos de A e x com 1.0 para teste
        for (int i = 0; i < M * N; i++) A[i] = 1.0;
        for (int i = 0; i < N; i++) x[i] = 1.0;
    }

    // Medição de tempo e definição de tipos derivador MPI
    double start = MPI_Wtime(); // Inicia a contagem do tempo de execução paralela

    // Criar tipo derivado para colunas intercaladas
    MPI_Datatype column_type, resized_col_type;     // Declaração dos tipos derivados

    // MPI_Type_vector: Cria um tipo que descreve um padrão de dados não contíguo.
    // M: número de blocos (linhas da matriz).
    // local_cols: tamanho de cada bloco (número de colunas que um processo receberá).
    // N: passo (stride) entre o início de um bloco e o início do próximo bloco.
    //    Como a matriz é row-major, para pular para a próxima linha e pegar o elemento da mesma coluna,
    //    precisamos pular N elementos (o comprimento total de uma linha).
    // MPI_DOUBLE: tipo de dado base.
    // &column_type: ponteiro para o novo tipo derivado.
    MPI_Type_vector(M, local_cols, N, MPI_DOUBLE, &column_type);

    // MPI_Type_create_resized: Ajusta os limites (lower_bound e extent) de um tipo derivado.
    // column_type: O tipo base a ser redimensionado.
    // 0: Novo lower_bound (offset inicial), geralmente 0.
    // local_cols * sizeof(double): Novo extent (tamanho total em bytes) do tipo.
    //    O MPI_Type_vector cria um tipo cujo extent é M * N * sizeof(double), que é o tamanho da matriz completa,
    //    pois ele inclui os "saltos" (strides).
    //    Para que MPI_Scatter entenda que o "pacote" de dados é local_cols * sizeof(double) por envio,
    //    e que o próximo envio começa local_cols * sizeof(double) depois, precisamos ajustar o extent.
    // &resized_col_type: ponteiro para o novo tipo derivado redimensionado.
    MPI_Type_create_resized(column_type, 0, local_cols * sizeof(double), &resized_col_type);

    // MPI_Type_commit: Finaliza a criação do tipo derivado, tornando-o utilizável para comunicação.
    MPI_Type_commit(&resized_col_type);

    // Distribuição de Dados

    // MPI_Scatter: Distribui os blocos de colunas da matriz A do processo raiz para todos os processos.
    // A: Buffer de envio no processo raiz (matriz completa).
    // 1: Quantidade de "elementos" de resized_col_type que cada processo receberá.
    //    Como resized_col_type representa um "bloco de colunas", cada processo recebe 1 desses blocos.
    // resized_col_type: O tipo de dado derivado que descreve o padrão das colunas a serem enviadas.
    // local_A: Buffer de recebimento em cada processo.
    // M * local_cols: Quantidade de elementos MPI_DOUBLE que cada processo espera receber.
    // MPI_DOUBLE: O tipo de dado dos elementos no buffer de recebimento.
    // 0: Rank do processo raiz que está enviando os dados.
    // MPI_COMM_WORLD: Comunicador.
    MPI_Scatter(A, 1, resized_col_type, local_A, M * local_cols, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // Distribuir pedaço correspondente de x
    if (rank == 0) {   // Apenas o processo raiz envia os segmentos de x
        for (int i = 0; i < size; i++) {
            if (i == 0)     // O próprio processo raiz pega seu segmento de x diretamente
                for (int j = 0; j < local_cols; j++) local_x[j] = x[j];
            else            // Para os outros processos, envia o segmento de x
                MPI_Send(&x[i * local_cols], local_cols, MPI_DOUBLE, i, 0, MPI_COMM_WORLD);
        }
    } else {    // Os processos trabalhadores recebem seus segmentos de x
        MPI_Recv(local_x, local_cols, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    // Cada processo calcula sua contribuição para o vetor y
    // Este é o cerne da computação paralela.
    for (int i = 0; i < M; i++) {   // Itera sobre as linhas da matriz (e elementos de y)
        for (int j = 0; j < local_cols; j++) {  // Itera sobre as colunas locais

            // local_y[i] acumula a soma dos produtos A_ij * x_j para as colunas locais.
            // local_A é acessado como uma matriz M x local_cols.
            local_y[i] += local_A[i * local_cols + j] * local_x[j];
        }
    }

    // Reduzir as contribuições parciais de todos os processos para y em rank 0
    // MPI_Reduce: Soma as contribuições parciais (local_y) de todos os processos no processo raiz (rank 0).
    // local_y: Buffer de envio de cada processo (sua contribuição parcial).
    // y: Buffer de recebimento no processo raiz (onde o resultado final será acumulado).
    // M: Número de elementos a serem reduzidos (tamanho do vetor y).
    // MPI_DOUBLE: Tipo de dado dos elementos.
    // MPI_SUM: A operação de redução (soma).
    // 0: Rank do processo raiz onde o resultado final será acumulado.
    // MPI_COMM_WORLD: Comunicador.
    MPI_Reduce(local_y, y, M, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    double end = MPI_Wtime();   // Finaliza a contagem do tempo
    double tempo = end - start; // Calcula o tempo total de execução

    // GRavação de resultados e limpeza
    if (rank == 0) {    // Apenas o processo mestre lida com a gravação dos resultados e libera a memória global
        FILE *fp = fopen("resultados.csv", "a");    // Abre o arquivo CSV em modo de adição
        if (fp != NULL) {   // Verifica se o arquivo foi aberto com sucesso
            // Escreve os resultados no CSV: M, N, Num_processos, Tempo_segundos
            fprintf(fp, "%d,%d,%d,%.6f\n", M, N, size, tempo);
            fclose(fp); // Fecha o arquivo
        } else {
            fprintf(stderr, "Erro ao abrir arquivo CSV.\n");    // Imprime erro se não conseguir abrir o arquivo
        }

        // Libera a memória alocada para as matrizes e vetores completos (apenas no processo raiz)
        free(A);
        free(x);
        free(y);
    }

    // Libera a memória alocada localmente por todos os processos
    free(local_A);
    free(local_x);
    free(local_y);

    // Libera os tipos derivados MPI criados
    MPI_Type_free(&column_type);
    MPI_Type_free(&resized_col_type);
    
    // Finaliza o ambiente MPI. Esta é a última chamada MPI no programa.
    MPI_Finalize();
    return 0;   // Retorna 0 indicando sucesso
}

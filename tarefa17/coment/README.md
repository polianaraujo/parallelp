# Código

Vamos destrinchar o código C fornecido, `matvec_col.c`, que implementa a multiplicação de matriz por vetor paralelizada com MPI, distribuindo a matriz por colunas.

---

## 1. Inclusão de Bibliotecas e Inicialização MPI

```c
#include <stdio.h>   // Para funções de entrada/saída (printf, fopen, fprintf, stderr)
#include <stdlib.h>  // Para alocação de memória (malloc, calloc, free) e conversão de string (atoi)
#include <mpi.h>     // Para as funções da biblioteca MPI (MPI_Init, MPI_Comm_rank, etc.)

int main(int argc, char *argv[]) {
    int rank, size, M, N; // rank: ID do processo; size: número total de processos; M, N: dimensões da matriz

    // Inicializa o ambiente MPI. Esta é a primeira chamada MPI em qualquer programa MPI.
    MPI_Init(&argc, &argv);

    // Obtém o rank (identificador único) do processo atual dentro do comunicador MPI_COMM_WORLD.
    // O rank 0 é geralmente o processo mestre.
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // Obtém o número total de processos no comunicador MPI_COMM_WORLD.
    MPI_Comm_size(MPI_COMM_WORLD, &size);
```

* **`#include`**: As diretivas de pré-processamento incluem as bibliotecas padrão de C para operações de I/O e manipulação de memória, além da biblioteca MPI para paralelização.
* **`main(int argc, char *argv[])`**: A função principal do programa. `argc` é o número de argumentos da linha de comando e `argv` é um array de strings contendo esses argumentos.
* **`int rank, size, M, N;`**: Declaração de variáveis inteiras para armazenar o ID do processo (`rank`), o número total de processos (`size`), e as dimensões da matriz $M$ (linhas) e $N$ (colunas).
* **`MPI_Init(&argc, &argv);`**: Esta é a primeira chamada MPI no programa. Ela inicializa o ambiente de execução MPI, configurando tudo o que é necessário para a comunicação entre os processos. `&argc` e `&argv` são passados para que o MPI possa processar seus próprios argumentos de linha de comando, se houver.
* **`MPI_Comm_rank(MPI_COMM_WORLD, &rank);`**: Esta função obtém o identificador único (rank) do processo atual dentro do comunicador `MPI_COMM_WORLD`. O comunicador `MPI_COMM_WORLD` é o comunicador padrão que inclui todos os processos iniciados. O `rank` de um processo é um inteiro de 0 a `size - 1`. O processo com `rank == 0` é frequentemente o processo "mestre" que coordena as operações.
* **`MPI_Comm_size(MPI_COMM_WORLD, &size);`**: Esta função obtém o número total de processos que fazem parte do comunicador `MPI_COMM_WORLD`.

---

## 2. Validação de Argumentos e Divisibilidade

```c
    if (argc != 3) { // Verifica se o número de argumentos está correto (nome do programa + M + N)
        if (rank == 0) // Apenas o processo mestre imprime a mensagem de uso
            printf("Uso: %s <M> <N>\n", argv[0]);
        MPI_Finalize(); // Finaliza o ambiente MPI antes de sair
        return -1;      // Retorna código de erro
    }

    M = atoi(argv[1]); // Converte o primeiro argumento (M) para inteiro
    N = atoi(argv[2]); // Converte o segundo argumento (N) para inteiro

    // Garante que o número de colunas (N) é divisível pelo número de processos (size).
    // Isso é essencial para uma distribuição uniforme das colunas.
    if (N % size != 0) {
        if (rank == 0) // Apenas o processo mestre imprime a mensagem de erro
            printf("Erro: N (%d) deve ser divisível pelo número de processos (%d).\n", N, size);
        MPI_Finalize(); // Finaliza o ambiente MPI antes de sair
        return -1;      // Retorna código de erro
    }

    int local_cols = N / size; // Calcula o número de colunas que cada processo irá manipular
```

* **Validação de `argc`**: O programa espera exatamente dois argumentos após o nome do executável (`M` e `N`). Se não forem fornecidos, ele imprime uma mensagem de uso (apenas pelo processo `rank == 0` para evitar mensagens duplicadas) e encerra o MPI (`MPI_Finalize`) antes de sair.
* **`atoi(argv[1])`, `atoi(argv[2])`**: Converte os argumentos da linha de comando, que são strings, para inteiros, atribuindo-os a `M` e `N`.
* **Verificação de Divisibilidade (`N % size != 0`)**: Esta é uma condição crucial para a estratégia de distribuição por colunas. Para que as colunas sejam distribuídas uniformemente entre os processos (cada processo recebe `N/size` colunas), `N` deve ser divisível por `size`. Se não for, o programa imprime um erro e encerra.
* **`int local_cols = N / size;`**: Calcula o número de colunas que cada processo será responsável por manipular.

---

## 3. Alocação de Memória

```c
    double *A = NULL; // Ponteiro para a matriz A (completa, apenas no processo raiz)
    double *x = NULL; // Ponteiro para o vetor x (completo, apenas no processo raiz)

    // local_A: Armazena o bloco de colunas da matriz A que este processo irá receber.
    // É alocado para todos os processos. Tamanho: M linhas * local_cols colunas.
    double *local_A = (double *)malloc(M * local_cols * sizeof(double));

    // local_x: Armazena o segmento do vetor x que este processo irá receber.
    // É alocado para todos os processos. Tamanho: local_cols elementos.
    double *local_x = (double *)malloc(local_cols * sizeof(double));

    // local_y: Armazena a contribuição parcial deste processo para o vetor resultado y.
    // É alocado para todos os processos e inicializado com zeros (calloc). Tamanho: M elementos.
    double *local_y = (double *)calloc(M, sizeof(double));

    double *y = NULL; // Ponteiro para o vetor resultado y (completo, apenas no processo raiz)

    if (rank == 0) { // Apenas o processo mestre (rank 0) aloca e inicializa as matrizes e vetores completos
        A = (double *)malloc(M * N * sizeof(double)); // Matriz A de M x N doubles
        x = (double *)malloc(N * sizeof(double));     // Vetor x de N doubles
        y = (double *)malloc(M * sizeof(double));     // Vetor resultado y de M doubles

        // Inicializa todos os elementos de A e x com 1.0 para teste
        for (int i = 0; i < M * N; i++) A[i] = 1.0;
        for (int i = 0; i < N; i++) x[i] = 1.0;
    }
```

* **`double *A = NULL;`, `double *x = NULL;`, `double *y = NULL;`**: Ponteiros para a matriz `A` e os vetores `x` e `y`. Eles são inicializados com `NULL` e só serão alocados no processo raiz (`rank == 0`).
* **`double *local_A = (double *)malloc(M * local_cols * sizeof(double));`**: Aloca memória para a porção local da matriz `A` que cada processo receberá. Como a matriz é distribuída por colunas, cada processo receberá `local_cols` colunas, cada uma com `M` elementos. A memória total é `M * local_cols * sizeof(double)`.
* **`double *local_x = (double *)malloc(local_cols * sizeof(double));`**: Aloca memória para a porção local do vetor `x` que cada processo receberá. O tamanho é `local_cols` elementos.
* **`double *local_y = (double *)calloc(M, sizeof(double));`**: Aloca memória para o vetor `local_y`, que armazenará a **contribuição parcial** de cada processo para o resultado final `y`. É importante usar `calloc` aqui, pois ele inicializa todos os bytes alocados com zero. Isso é crucial porque `local_y` será usado como um acumulador (`local_y[i] += ...`).
* **`if (rank == 0)`**: O bloco de código dentro deste `if` é executado apenas pelo processo mestre. Ele aloca as versões completas da matriz `A` e dos vetores `x` e `y`, e as inicializa com valores de 1.0 para simplificar o teste.

---

## 4. Medição de Tempo e Definição de Tipos Derivados MPI

```c
    double start = MPI_Wtime(); // Inicia a contagem do tempo de execução paralela

    // Criar tipo derivado para colunas intercaladas
    MPI_Datatype column_type, resized_col_type; // Declaração dos tipos derivados

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
```

* **`double start = MPI_Wtime();`**: `MPI_Wtime()` retorna o tempo de relógio de parede em segundos. Ele é usado para medir o tempo de execução da parte paralela do algoritmo.
* **`MPI_Datatype column_type, resized_col_type;`**: Declaração de duas variáveis do tipo `MPI_Datatype` para armazenar os tipos derivados.
* **`MPI_Type_vector(M, local_cols, N, MPI_DOUBLE, &column_type);`**: Esta função cria um tipo de dado derivado que descreve o layout de **colunas** em uma matriz armazenada em *row-major order*.
    * `M`: É o número de blocos. Como estamos pegando colunas, há $M$ "fatias" ou "blocos" de dados (um para cada linha).
    * `local_cols`: É o número de elementos em cada bloco. Cada processo recebe `local_cols` colunas.
    * `N`: É o *stride* (passo) entre o início de um bloco e o início do próximo. Como a matriz é *row-major*, para ir da primeira linha de um grupo de colunas para a próxima linha do mesmo grupo, precisamos pular $N$ elementos (o comprimento total de uma linha).
    * `MPI_DOUBLE`: O tipo de dado base dos elementos.
    * `&column_type`: O identificador para o tipo derivado que será criado.
* **`MPI_Type_create_resized(column_type, 0, local_cols * sizeof(double), &resized_col_type);`**: Esta função é essencial para corrigir o "extent" do `column_type`. O `MPI_Type_vector` define um tipo que, por padrão, tem um "extent" igual ao tamanho da matriz completa (`M * N * sizeof(double)`) porque ele considera os "saltos" (`N`). No entanto, para `MPI_Scatter` funcionar corretamente, o "extent" de cada "pedaço" de dado deve ser o tamanho real de `local_cols` doubles (o tamanho de um grupo de colunas em uma linha). Esta função ajusta o `lower_bound` para 0 e define o novo `extent` para `local_cols * sizeof(double)`.
* **`MPI_Type_commit(&resized_col_type);`**: Após definir um tipo derivado, ele deve ser "commitado" (finalizado) para que o MPI possa usá-lo em operações de comunicação.

---

## 5. Distribuição de Dados

```c
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
    if (rank == 0) { // Apenas o processo raiz envia os segmentos de x
        for (int i = 0; i < size; i++) {
            if (i == 0) // O próprio processo raiz pega seu segmento de x diretamente
                for (int j = 0; j < local_cols; j++) local_x[j] = x[j];
            else // Para os outros processos, envia o segmento de x
                MPI_Send(&x[i * local_cols], local_cols, MPI_DOUBLE, i, 0, MPI_COMM_WORLD);
        }
    } else { // Os processos trabalhadores recebem seus segmentos de x
        MPI_Recv(local_x, local_cols, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
```

* **`MPI_Scatter(A, 1, resized_col_type, local_A, M * local_cols, MPI_DOUBLE, 0, MPI_COMM_WORLD);`**: Esta é a chamada que distribui a matriz `A`.
    * `A`: O buffer de envio (a matriz completa) no processo raiz (`rank == 0`).
    * `1`: Cada processo receberá `1` item do tipo `resized_col_type`.
    * `resized_col_type`: O tipo de dado derivado que define o padrão das colunas a serem enviadas. Graças ao `MPI_Type_create_resized`, o `MPI_Scatter` consegue "avançar" corretamente na memória de `A` para pegar o próximo bloco de colunas.
    * `local_A`: O buffer onde cada processo armazenará sua parte da matriz `A`.
    * `M * local_cols`: O número total de elementos `double` que cada processo espera receber.
    * `MPI_DOUBLE`: O tipo de dado real dos elementos recebidos.
    * `0`: O processo raiz que está enviando os dados.
    * `MPI_COMM_WORLD`: O comunicador.
* **Distribuição do vetor `x` (manual com `MPI_Send`/`MPI_Recv`)**:
    * O processo `rank == 0` itera sobre todos os processos. Para ele mesmo (`i == 0`), ele simplesmente copia a porção inicial de `x` para `local_x`. Para os outros processos (`i > 0`), ele usa `MPI_Send` para enviar o segmento correto de `x` (`&x[i * local_cols]`) para o processo `i`.
    * Os processos trabalhadores (`rank != 0`) usam `MPI_Recv` para receber seu segmento de `x` do processo raiz.
    * **Observação**: Embora funcional, a distribuição do vetor `x` poderia ser feita de forma mais elegante e possivelmente mais eficiente (especialmente para um grande número de processos) usando `MPI_Scatter` também para `x`. Por exemplo: `MPI_Scatter(x, local_cols, MPI_DOUBLE, local_x, local_cols, MPI_DOUBLE, 0, MPI_COMM_WORLD);`.

---

## 6. Cálculo Local e Redução Final

```c
    // Cada processo calcula sua contribuição para o vetor y
    // Este é o cerne da computação paralela.
    for (int i = 0; i < M; i++) { // Itera sobre as linhas da matriz (e elementos de y)
        for (int j = 0; j < local_cols; j++) { // Itera sobre as colunas locais
            // local_y[i] acumula a soma dos produtos A_ij * x_j para as colunas locais.
            // local_A é acessado como uma matriz M x local_cols.
            local_y[i] += local_A[i * local_cols + j] * local_x[j];
        }
    }

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
```

* **Loops de Cálculo (`for (int i ...)` e `for (int j ...)` )**: Esta seção é onde a multiplicação matriz-vetor é efetivamente realizada.
    * O loop externo (`i`) itera sobre as $M$ linhas da matriz (e, consequentemente, sobre os $M$ elementos do vetor `y`).
    * O loop interno (`j`) itera sobre as `local_cols` colunas que este processo possui.
    * `local_y[i] += local_A[i * local_cols + j] * local_x[j];`: Esta linha é a operação central. Para cada elemento `y[i]`, o processo acumula a soma dos produtos dos elementos da linha $i$ de `local_A` com os elementos correspondentes de `local_x`. Observe que `local_A` é acessado de forma contígua em `j`, o que é bom para o cache, pois as colunas que foram distribuídas para este processo estão agora armazenadas sequencialmente.
* **`MPI_Reduce(local_y, y, M, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);`**: Após cada processo ter calculado sua contribuição parcial (`local_y`), esta função realiza uma operação de redução global.
    * Todos os processos enviam seu `local_y` para o processo raiz (`rank == 0`).
    * A operação `MPI_SUM` é aplicada elemento a elemento: `y[i] = local_y_0[i] + local_y_1[i] + ... + local_y_P-1[i]`.
    * O resultado final é armazenado no vetor `y` **apenas no processo raiz (`rank == 0`)**.

---

## 7. Gravação de Resultados e Limpeza

```c
    if (rank == 0) { // Apenas o processo mestre lida com a gravação dos resultados e libera a memória global
        FILE *fp = fopen("resultados.csv", "a"); // Abre o arquivo CSV em modo de adição
        if (fp != NULL) { // Verifica se o arquivo foi aberto com sucesso
            // Escreve os resultados no CSV: M, N, Num_processos, Tempo_segundos
            fprintf(fp, "%d,%d,%d,%.6f\n", M, N, size, tempo);
            fclose(fp); // Fecha o arquivo
        } else {
            fprintf(stderr, "Erro ao abrir arquivo CSV.\n"); // Imprime erro se não conseguir abrir o arquivo
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
    return 0; // Retorna 0 indicando sucesso
}
```

* **`if (rank == 0)`**: O bloco de código aqui é executado apenas pelo processo mestre.
    * **Gravação em CSV**: Ele abre um arquivo chamado `resultados.csv` no modo de adição (`"a"`), o que significa que se o arquivo existir, os novos dados serão anexados ao final. Ele então escreve os valores de $M$, $N$, número de processos e o tempo de execução no arquivo, formatados como CSV.
    * **Liberação de Memória Global**: `free(A); free(x); free(y);` libera a memória que foi alocada **apenas pelo processo raiz** para as matrizes e vetores completos.
* **`free(local_A); free(local_x); free(local_y);`**: Estas chamadas `free` são executadas por **todos os processos** para liberar a memória de suas porções locais.
* **`MPI_Type_free(&column_type); MPI_Type_free(&resized_col_type);`**: É crucial liberar os tipos de dados derivados MPI quando eles não são mais necessários para evitar vazamentos de memória e recursos MPI.
* **`MPI_Finalize();`**: Esta é a última chamada MPI. Ela encerra o ambiente MPI, liberando todos os recursos MPI usados pelos processos.
* **`return 0;`**: Indica que o programa foi executado com sucesso.

Em resumo, este código é uma implementação robusta e bem estruturada de multiplicação de matriz por vetor usando MPI com distribuição por colunas, que faz uso avançado de tipos de dados derivados para lidar eficientemente com o layout da memória.
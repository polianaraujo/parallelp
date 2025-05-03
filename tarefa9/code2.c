// Parte 2: Generalizando para M listas (definido pelo usuário)
// Quando o número de listas é arbitrário (definido em tempo de execução), precisamos de um array de listas e um mutex para cada uma. Criamos um array de mutexes também.

// Problema com regiões críticas nomeadas:
// Regiões críticas nomeadas (com mutexes independentes) funcionam bem enquanto cada thread acessa apenas uma estrutura compartilhada por vez.

// Mas, se uma thread precisar acessar mais de uma lista ao mesmo tempo (ex: mover elementos entre listas), pode haver deadlocks se os locks forem adquiridos fora de ordem.

// Com múltiplas listas e múltiplas threads, o uso de locks explícitos e cuidadoso controle da ordem de aquisição dos locks se torna essencial para evitar essas condições.


// Versão 2: Número arbitrário de listas com locks explícitos

// Agora generalizamos para K listas. As regiões críticas nomeadas não bastam, pois não podemos gerar dinamicamente nomes de regiões críticas. Logo, usamos locks explícitos:

#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>

typedef struct Node {
    int value;
    struct Node* next;
} Node;

void insert(Node** head, int value) {
    Node* new_node = (Node*) malloc(sizeof(Node));
    new_node->value = value;
    new_node->next = *head;
    *head = new_node;
}

void print_list(Node* head) {
    Node* current = head;
    while (current) {
        printf("%d -> ", current->value);
        current = current->next;
    }
    printf("NULL\n");
}

int main() {
    int N = 100;
    int K;

    printf("Digite o número de listas: ");
    scanf("%d", &K);

    Node** lists = malloc(K * sizeof(Node*));
    omp_lock_t* locks = malloc(K * sizeof(omp_lock_t));

    for (int i = 0; i < K; i++) {
        lists[i] = NULL;
        omp_init_lock(&locks[i]);
    }

    srand(time(NULL));

    #pragma omp parallel for
    for (int i = 0; i < N; i++) {
        int value = rand() % 1000;
        int list_index = rand() % K;

        omp_set_lock(&locks[list_index]);
        insert(&lists[list_index], value);
        omp_unset_lock(&locks[list_index]);
    }

    for (int i = 0; i < K; i++) {
        printf("Lista %d: ", i);
        print_list(lists[i]);
        omp_destroy_lock(&locks[i]);
    }

    free(lists);
    free(locks);

    return 0;
}

// Por que locks explícitos são necessários na versão com K listas?

// Porque o #pragma omp critical(nome) exige nomes constantes e fixos em tempo de compilação, ou seja, você não pode criar N nomes dinâmicos como list0_crit, list1_crit, ..., listK_crit com uma variável. Já com locks explícitos, você pode usar um array de locks, e indexá-lo dinamicamente conforme necessário, permitindo a concorrência segura entre múltiplas listas.



// Explicação técnica (resumo)
// Com duas listas: é fácil garantir integridade com dois mutexes.

// Com M listas arbitrárias: o uso de regiões críticas nomeadas ainda funciona, mas se futuramente for necessário:

// - acessar múltiplas listas simultaneamente,

// - ou realizar operações mais complexas entre listas,

// será necessário usar locks explícitos, adquiridos em uma ordem fixa para evitar deadlocks.


// gcc -fopenmp -o listas listas_openmp.c

// -fopenmp: ativa o suporte a OpenMP.
// -o listas: o executável se chamará listas.

// ./listas



// Dica extra: escolher o número de threads
// Você pode definir quantas threads paralelas quer usar com a variável de ambiente OMP_NUM_THREADS.
// OMP_NUM_THREADS=4 ./listas
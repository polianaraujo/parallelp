// Escreva um programa que cria tarefas para realizar N inserções em duas listas encadeadas, cada uma associada a uma thread. Cada tarefa deve escolher aleatoriamente em qual lista inserir um número. Garanta a integridade das listas evitando condição de corrida e, sempre que possível, use regiões críticas nomeadas para que a inserção em uma lista não bloqueie a outra. Em seguida, generalize o programa para um número de listas definido pelo usuário. Explique por que, nesse caso, regiões críticas nomeadas não são suficientes e por que o uso de locks explícitos se torna necessário.

/*

pragma omp critical

locks explícitos:
omp_lock_t lock
omp_init_lock (&lock)
omp_set_lock (&lock)
omp_unset_lock (&lock)
omp_destroy_lock (&lock)

*/

// Parte 1: Duas listas encadeadas com threads e regiões críticas nomeadas
// Vamos usar pthread para threads e pthread_mutex_t para os locks. Como são apenas duas listas, podemos criar um mutex para cada lista, evitando que threads bloqueiem umas às outras ao operar em listas diferentes.

// Conceito de Região Crítica Nomeada
// Cada lista tem seu próprio lock. Assim, se uma thread está inserindo na Lista A, a Lista B ainda pode ser acessada por outra thread. Isso é chamado de região crítica nomeada porque o recurso (a lista) tem um lock "nomeado" ou específico.

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
    Node* list1 = NULL;
    Node* list2 = NULL;
    int N = 100;

    srand(time(NULL));

    #pragma omp parallel for
    for (int i = 0; i < N; i++) {
        int value = rand() % 1000;
        int list_choice = rand() % 2;

        if (list_choice == 0) {
            #pragma omp critical(list1_crit)
            insert(&list1, value);
        } else {
            #pragma omp critical(list2_crit)
            insert(&list2, value);
        }
    }

    printf("Lista 1: ");
    print_list(list1);
    printf("Lista 2: ");
    print_list(list2);

    return 0;
}

// Por que critical(nome) funciona bem aqui?

// Porque há duas listas independentes, então usamos duas regiões críticas nomeadas diferentes (list1_crit e list2_crit). Isso permite que uma thread insira em list1 enquanto outra insere em list2 simultaneamente, sem colisões.